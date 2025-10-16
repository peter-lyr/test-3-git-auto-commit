import os
import sys
import subprocess
from collections import defaultdict

global commit_info_file

commit_info_file = ""


def get_git_status():
    """获取git状态信息，返回修改和未跟踪的文件列表"""
    try:
        result = subprocess.run(
            ["git", "status", "--porcelain"], capture_output=True, text=True, check=True
        )
        lines = result.stdout.strip().split("\n") if result.stdout.strip() else []

        file_list = []
        for line in lines:
            if line.strip():
                # 解析状态和文件名
                status = line[:2].strip()
                filename = line[3:].strip()

                # 处理重命名的情况
                if " -> " in filename:
                    filename = filename.split(" -> ")[1]

                # 去除文件名两端的引号
                if filename.startswith('"') and filename.endswith('"'):
                    filename = filename[1:-1]

                file_list.append(filename)

        return file_list
    except subprocess.CalledProcessError as e:
        print(f"Error getting git status: {e}")
        return []
    except Exception as e:
        print(f"Unexpected error: {e}")
        return []


def scan_directory_recursive(path):
    """递归扫描目录，返回所有文件的路径和大小"""
    file_sizes = []

    if os.path.isfile(path):
        # 如果是文件，直接返回文件大小
        try:
            size = os.path.getsize(path)
            file_sizes.append((path, size))
        except OSError:
            pass
    elif os.path.isdir(path):
        # 如果是目录，递归扫描
        try:
            for root, dirs, files in os.walk(path):
                for file in files:
                    file_path = os.path.join(root, file)
                    try:
                        size = os.path.getsize(file_path)
                        file_sizes.append((file_path, size))
                    except OSError:
                        continue
        except OSError:
            pass

    return file_sizes


def get_file_size_mb(file_path):
    """获取文件大小（MB）"""
    try:
        return os.path.getsize(file_path) / (1024 * 1024)
    except OSError:
        return 0


def scan_and_categorize_files():
    """扫描并分类文件"""
    git_files = get_git_status()
    if not git_files:
        print("No modified or untracked files found.")
        return {}

    # 收集所有需要处理的文件路径
    all_files = []
    for file_path in git_files:
        print(
            "## file_path:",
            file_path,
            ", os.path.isdir(file_path)",
            os.path.isdir(file_path),
        )
        if os.path.isdir(file_path):
            # 递归扫描目录
            dir_files = scan_directory_recursive(file_path)
            print("## file_path:", file_path)
            print("## len(dir_files):", len(dir_files))
            for f_path, size_bytes in dir_files:
                size_mb = size_bytes / (1024 * 1024)
                all_files.append((f_path, size_mb))
        else:
            all_files.append((file_path, get_file_size_mb(file_path)))

    if not all_files:
        print("No valid files to process.")
        return {}

    # 按目录分组，过滤掉空目录
    dir_files_map = defaultdict(list)
    for file_path, size_mb in all_files:
        dir_path = os.path.dirname(file_path)
        # 过滤掉空目录名
        if dir_path:
            dir_files_map[dir_path].append((file_path, size_mb))

    # 计算目录深度
    dir_depths = {}
    for dir_path in dir_files_map.keys():
        depth = dir_path.count(os.sep)
        dir_depths[dir_path] = depth

    # 按深度排序（从深到浅）
    sorted_dirs = sorted(dir_depths.keys(), key=lambda x: dir_depths[x], reverse=True)

    result_dict = {}
    processed_files = set()

    # 从最深目录开始处理
    for dir_path in sorted_dirs:
        # 检查是否所有文件都已被处理
        current_files = [
            (f, s) for f, s in dir_files_map[dir_path] if f not in processed_files
        ]
        if not current_files:
            continue

        total_size = sum(size for _, size in current_files)

        if total_size <= 50:
            # 整个目录添加到结果
            result_dict[dir_path] = total_size
            # 标记这些文件为已处理
            for file_path, _ in current_files:
                processed_files.add(file_path)
        else:
            # 单独添加文件
            for file_path, size_mb in current_files:
                if file_path not in processed_files:
                    result_dict[file_path] = size_mb
                    processed_files.add(file_path)

    # 处理不在任何目录中的文件（根目录下的文件）
    for file_path, size_mb in all_files:
        # 根目录下的文件（没有父目录或父目录为空）
        dir_name = os.path.dirname(file_path)
        if not dir_name and file_path not in processed_files:
            result_dict[file_path] = size_mb
            processed_files.add(file_path)

    return result_dict


def execute_git_commands(files_dict):
    """执行git命令"""
    if not files_dict:
        print("No files to commit.")
        return False

    # 打印结果字典
    print("Files/Folders to be committed:")
    total_size = 0
    for path, size in files_dict.items():
        print(f"  {path}: {size:.2f} MB")
        total_size += size

    print(f"\nTotal size: {total_size:.2f} MB")

    if total_size <= 100:
        # 直接提交所有文件
        print("\nTotal size <= 100MB, committing all files at once...")
        try:
            print("> git add -A")
            exit_code = os.system("git add -A")
            if exit_code != 0:
                print(f"git add -A failed with exit code: {exit_code}")

            print(f"> git commit -F {commit_info_file}")
            exit_code = os.system(f"git commit -F {commit_info_file}")
            if exit_code != 0:
                print(f"git commit failed with exit code: {exit_code}")

            print("> git push")
            exit_code = os.system("git push")
            if exit_code != 0:
                print(f"git push failed with exit code: {exit_code}")

            return True
        except Exception as e:
            print(f"Error executing git commands: {e}")
            return False
    else:
        # 分批提交
        print("\nTotal size > 100MB, committing in batches...")

        current_batch = []
        current_batch_size = 0

        for path, size in files_dict.items():
            # 跳过单个文件超过100MB的
            if size > 100:
                print(f"Skipping large file: {path} ({size:.2f} MB)")
                continue

            if current_batch_size + size <= 100:
                current_batch.append(path)
                current_batch_size += size
            else:
                # 提交当前批次
                if current_batch:
                    commit_batch(current_batch, current_batch_size)
                    current_batch = []
                    current_batch_size = 0

                # 开始新批次
                current_batch.append(path)
                current_batch_size = size

        # 提交最后一批
        if current_batch:
            commit_batch(current_batch, current_batch_size)

        return True


def commit_batch(file_paths, batch_total_size):
    """提交一个批次的文件"""
    print(
        f"\nCommitting batch ({len(file_paths)} files, total size: {batch_total_size:.2f} MB)..."
    )

    # 打印批次中每个文件/文件夹的大小
    print("Batch contents:")
    for file_path in file_paths:
        # 计算文件/文件夹的大小
        if os.path.isdir(file_path):
            # 如果是文件夹，计算文件夹下所有文件的总大小
            dir_size = 0
            for root, dirs, files in os.walk(file_path):
                for file in files:
                    file_path_full = os.path.join(root, file)
                    try:
                        dir_size += os.path.getsize(file_path_full)
                    except OSError:
                        pass
            dir_size_mb = dir_size / (1024 * 1024)
            print(f"  {file_path}: {dir_size_mb:.2f} MB (folder)")
        else:
            # 如果是文件，直接获取大小
            file_size_mb = get_file_size_mb(file_path)
            print(f"  {file_path}: {file_size_mb:.2f} MB")

    try:
        # git add 每个文件
        for file_path in file_paths:
            print(f"> git add {file_path}")
            # 对文件路径进行转义，防止特殊字符问题
            escaped_path = file_path.replace('"', '\\"')
            exit_code = os.system(f'git add "{escaped_path}"')
            if exit_code != 0:
                print(f"git add failed for {file_path} with exit code: {exit_code}")

        # git commit
        print(f"> git commit -F {commit_info_file}")
        exit_code = os.system(f"git commit -F {commit_info_file}")
        if exit_code != 0:
            print(f"git commit failed with exit code: {exit_code}")

        # git push
        print("> git push")
        exit_code = os.system("git push")
        if exit_code != 0:
            print(f"git push failed with exit code: {exit_code}")

    except Exception as e:
        print(f"Error committing batch: {e}")


def main():
    global commit_info_file
    if len(sys.argv) != 2:
        print("Usage: python script.py <commit-info.txt>")
        sys.exit(1)

    commit_info_file = sys.argv[1]

    # 检查提交信息文件是否存在
    if not os.path.exists(commit_info_file):
        print(f"Error: Commit info file '{commit_info_file}' not found.")
        sys.exit(1)

    # 扫描和分类文件
    files_dict = scan_and_categorize_files()

    if not files_dict:
        print("No files to process.")
        return

    # 执行git命令
    success = execute_git_commands(files_dict)

    if success:
        print("\nAll operations completed successfully!")
    else:
        print("\nSome operations failed.")
        sys.exit(1)


if __name__ == "__main__":
    main()

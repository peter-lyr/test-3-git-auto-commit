import os
import sys
import subprocess
from collections import defaultdict

commit_info_file = ""


def get_git_status():
    """获取git状态信息，返回修改、未跟踪和已删除的文件列表"""
    try:
        result = subprocess.run(
            ["git", "status", "--porcelain"],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=True,
        )
        lines = result.stdout.strip().split("\n") if result.stdout.strip() else []

        file_list, deleted_files = [], []
        for line in lines:
            if not line.strip():
                continue

            status, filename = line[:2].strip(), line[3:].strip()
            if " -> " in filename:
                filename = filename.split(" -> ")[1]
            if filename.startswith('"') and filename.endswith('"'):
                filename = filename[1:-1]

            if "D" in status:
                deleted_files.append(filename)
            else:
                file_list.append(filename)

        return file_list, deleted_files
    except Exception as e:
        print(f"❌ Error getting git status: {e}")
        return [], []


def scan_directory_recursive(path):
    """递归扫描目录，返回所有文件的路径和大小"""
    file_sizes = []
    try:
        if os.path.isfile(path):
            file_sizes.append((path, os.path.getsize(path)))
        elif os.path.isdir(path):
            for root, _, files in os.walk(path):
                for file in files:
                    file_path = os.path.join(root, file)
                    try:
                        file_sizes.append((file_path, os.path.getsize(file_path)))
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
    git_files, deleted_files = get_git_status()

    if deleted_files:
        print(f"🗑️  Found {len(deleted_files)} deleted files")
        for file_path in deleted_files:
            print(f"   {file_path}")

    if not git_files and not deleted_files:
        print("📭 No modified, untracked or deleted files found")
        return {}, []

    # 收集所有需要处理的文件路径
    all_files = []
    for file_path in git_files:
        if os.path.isdir(file_path):
            for f_path, size_bytes in scan_directory_recursive(file_path):
                all_files.append((f_path, size_bytes / (1024 * 1024)))
        else:
            all_files.append((file_path, get_file_size_mb(file_path)))

    if not all_files and not deleted_files:
        print("📭 No valid files to process")
        return {}, []

    # 按目录分组并计算深度
    dir_files_map = defaultdict(list)
    for file_path, size_mb in all_files:
        if dir_path := os.path.dirname(file_path):
            dir_files_map[dir_path].append((file_path, size_mb))

    # 按深度排序（从深到浅）
    sorted_dirs = sorted(
        dir_files_map.keys(), key=lambda x: x.count(os.sep), reverse=True
    )

    result_dict, processed_files = {}, set()

    # 从最深目录开始处理
    for dir_path in sorted_dirs:
        current_files = [
            (f, s) for f, s in dir_files_map[dir_path] if f not in processed_files
        ]
        if not current_files:
            continue

        total_size = sum(size for _, size in current_files)

        if total_size <= 50:  # 整个目录添加到结果
            result_dict[dir_path] = total_size
            for file_path, _ in current_files:
                processed_files.add(file_path)
        else:  # 单独添加文件
            for file_path, size_mb in current_files:
                if file_path not in processed_files:
                    result_dict[file_path] = size_mb
                    processed_files.add(file_path)

    # 处理根目录下的文件
    for file_path, size_mb in all_files:
        if not os.path.dirname(file_path) and file_path not in processed_files:
            result_dict[file_path] = size_mb

    return result_dict, deleted_files


def execute_git_commands(files_dict, deleted_files):
    """执行git命令"""
    if not files_dict and not deleted_files:
        print("📭 No files to commit")
        return False

    # 计算总大小
    total_size = sum(files_dict.values()) if files_dict else 0

    print(f"📊 Total size: {total_size:.2f} MB")
    if deleted_files:
        print(f"🗑️  Deleted files: {len(deleted_files)}")

    # 如果总大小≤100MB或只有已删除的文件，一次性提交
    if total_size <= 100 or (total_size == 0 and deleted_files):
        print("\n🚀 Committing all files at once...")
        success = run_git_commands(
            ["git add -A", f"git commit -F {commit_info_file}", "git push"]
        )
        return success
    else:
        print("\n📦 Total size > 100MB, committing in batches...")
        return commit_in_batches(files_dict, deleted_files, total_size)


def commit_in_batches(files_dict, deleted_files, total_size):
    """分批提交文件"""
    # 计算总批次
    total_batches, current_batch, current_size = 0, [], 0
    for path, size in files_dict.items():
        if size > 100:
            continue
        if current_size + size <= 100:
            current_batch.append(path)
            current_size += size
        else:
            total_batches += 1
            current_batch, current_size = [path], size
    if current_batch:
        total_batches += 1

    print(f"📋 Total batches: {total_batches}")

    # 实际提交
    current_batch, current_size, committed_size, batch_num = [], 0, 0, 0

    for path, size in files_dict.items():
        if size > 100:
            print(f"⏭️  Skipping large file: {path} ({size:.2f} MB)")
            continue

        if current_size + size <= 100:
            current_batch.append(path)
            current_size += size
        else:
            if current_batch:
                batch_num += 1
                commit_batch(current_batch, current_size, batch_num, total_batches)
                committed_size += current_size
                print(f"✅ Completed Batch {batch_num}/{total_batches}")
                print(
                    f"📈 Progress: {committed_size:.2f}/{total_size:.2f} MB ({committed_size/total_size*100:.1f}%)"
                )
                current_batch, current_size = [], 0

            current_batch.append(path)
            current_size = size

    # 提交最后一批
    if current_batch:
        batch_num += 1
        commit_batch(current_batch, current_size, batch_num, total_batches)
        committed_size += current_size
        print(f"✅ Completed Batch {batch_num}/{total_batches}")
        print(
            f"📈 Progress: {committed_size:.2f}/{total_size:.2f} MB ({committed_size/total_size*100:.1f}%)"
        )

    # 提交已删除的文件
    if deleted_files:
        print("\n🗑️  Committing deleted files...")
        run_git_commands(
            ["git add -u", f"git commit -F {commit_info_file}", "git push"]
        )

    return True


def commit_batch(file_paths, batch_total_size, batch_number, total_batches):
    """提交一个批次的文件"""
    print(
        f"📦 Batch {batch_number}/{total_batches}: {len(file_paths)} files, {batch_total_size:.2f} MB"
    )

    # 验证路径并过滤无效路径
    valid_paths = []
    for file_path in file_paths:
        if not os.path.exists(file_path):
            print(f"⚠️  Path does not exist: {file_path}")
            continue

        if os.path.isdir(file_path):
            dir_size, file_count = 0, 0
            for root, _, files in os.walk(file_path):
                for file in files:
                    try:
                        dir_size += os.path.getsize(os.path.join(root, file))
                        file_count += 1
                    except OSError:
                        pass

            if file_count == 0:
                print(f"⚠️  Directory is empty: {file_path}")
                continue

            print(f"   {file_path}: {dir_size/(1024*1024):.2f} MB ({file_count} files)")
            valid_paths.append(file_path)
        else:
            print(f"   {file_path}: {get_file_size_mb(file_path):.2f} MB")
            valid_paths.append(file_path)

    if not valid_paths:
        print("⏭️  No valid paths, skipping batch")
        return False

    # 添加文件到Git
    successful_adds = 0
    for file_path in valid_paths:
        normalized_path = os.path.normpath(file_path)
        if not os.path.exists(normalized_path):
            print(f"⚠️  Path no longer exists: {normalized_path}")
            continue

        exit_code = os.system(f'git add "{normalized_path}"')
        if exit_code == 0:
            successful_adds += 1
        else:
            print(f"❌ git add failed: {normalized_path}")

    if successful_adds == 0:
        print("⏭️  No files added, skipping commit")
        return False

    print(f"✅ Added {successful_adds}/{len(valid_paths)} files")

    # 提交和推送
    return run_git_commands([f"git commit -F {commit_info_file}", "git push"])


def run_git_commands(commands):
    """执行一系列Git命令"""
    success = True
    for cmd in commands:
        print(f"▶️  {cmd}")
        exit_code = os.system(cmd)
        if exit_code != 0:
            print(f"❌ Command failed with exit code: {exit_code}")
            success = False
            # 不立即返回，继续执行其他命令
    return success


def main():
    global commit_info_file
    if len(sys.argv) != 2:
        print("❌ Usage: python script.py <commit-info.txt>")
        sys.exit(1)

    commit_info_file = sys.argv[1]
    if not os.path.exists(commit_info_file):
        print(f"❌ Commit info file '{commit_info_file}' not found")
        sys.exit(1)

    # 扫描和分类文件
    files_dict, deleted_files = scan_and_categorize_files()
    if not files_dict and not deleted_files:
        print("📭 No files to process")
        return

    # 执行git命令
    success = execute_git_commands(files_dict, deleted_files)
    if success:
        print("\n🎉 All operations completed successfully!")
    else:
        print("\n❌ Some operations failed")
        sys.exit(1)


if __name__ == "__main__":
    main()

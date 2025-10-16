# python需求：
# 传递一个参数
# py文件 commit-info.txt
# git add, git commit -F commit-info.txt, git push
# 首先检查所有已修改和未跟踪的文件的总大小
# 如果超过100M则分多次git add commit push，每次git add的所有文件总大小不超过100M
# 开始git add commit push之前，先检查所有已修改和未跟踪文件是否有超过50M的
# 如果是，则把它们所有都拆分成小文件，在它所在目录创建.gitignore文件，把原文件名添加进去
# 同时它所在目录如果不存在git-merge-split-files.exe，则将py文件同目录下的git-merge-split-files.exe文件复制过去
# 把原文件名（不包括拓展名）+".merged"添加进.gitignore文件中去
# 拆开成小文件完成之后，再扫描一下所有已修改和未跟踪的文件
# 一个批次中，每次git add的参数以文件夹为优先，
# 强调要求，只有当一个文件夹内所有的文件的总大小超过100M时，才一个一个文件去git add
# 一个批次的所有git add的所有文件和所有文件夹内的所有文件的总大小不超过100M
# git add commit push用os.system去跑，并且打印命令出来，其他的命令可以用subprocess.run
# 注意路径拼接的问题，不要拼接错了
# 注意获取文件夹的大小时要递归获取，不然直接返回4K大小了


import os
import sys
import subprocess
import shutil
from pathlib import Path


def run_command(cmd, capture_output=True):
    """运行命令并返回结果"""
    print(f"执行命令: {cmd}")
    if capture_output:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result
    else:
        result = subprocess.run(cmd, shell=True)
        return result.returncode == 0


def get_git_status_files():
    """获取所有已修改和未跟踪的文件"""
    # 获取已修改和未暂存的文件
    result = run_command("git status --porcelain")
    files = []

    for line in result.stdout.strip().split("\n"):
        if not line.strip():
            continue
        status = line[:2].strip()
        filepath = line[3:].strip()

        # 包括所有已修改、未跟踪的文件（排除已删除的文件）
        if status != "D":  # 忽略已删除的文件
            files.append(filepath)

    return files


def get_file_size(filepath):
    """获取文件大小（字节）"""
    try:
        return os.path.getsize(filepath)
    except OSError:
        return 0


def get_folder_size(folder_path):
    """递归获取文件夹大小（字节）"""
    total_size = 0
    for dirpath, dirnames, filenames in os.walk(folder_path):
        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            try:
                total_size += os.path.getsize(filepath)
            except OSError:
                continue
    return total_size


def split_large_file(filepath, max_size=50 * 1024 * 1024):
    """拆分大文件为小文件"""
    print(f"正在拆分大文件: {filepath}")

    file_size = get_file_size(filepath)
    if file_size <= max_size:
        return False

    # 获取文件信息
    file_path = Path(filepath)
    parent_dir = file_path.parent
    filename = file_path.name
    stem = file_path.stem

    # 检查是否需要复制合并工具
    merge_tool = "git-merge-split-files.exe"
    current_script_dir = Path(__file__).parent
    merge_tool_source = current_script_dir / merge_tool

    if merge_tool_source.exists() and not (parent_dir / merge_tool).exists():
        print(f"复制合并工具到: {parent_dir}")
        shutil.copy2(merge_tool_source, parent_dir / merge_tool)

    # 创建或更新.gitignore文件
    gitignore_path = parent_dir / ".gitignore"
    ignored_files = set()

    if gitignore_path.exists():
        with open(gitignore_path, "r", encoding="utf-8") as f:
            ignored_files = set(line.strip() for line in f if line.strip())

    # 添加原文件和合并文件到.gitignore
    ignored_files.add(filename)
    ignored_files.add(f"{stem}.merged")

    with open(gitignore_path, "w", encoding="utf-8") as f:
        for ignored_file in sorted(ignored_files):
            f.write(f"{ignored_file}\n")

    # 拆分文件
    part_num = 1
    bytes_written = 0

    with open(filepath, "rb") as original_file:
        while True:
            part_filename = f"{filename}.part{part_num:03d}"
            part_filepath = parent_dir / part_filename

            with open(part_filepath, "wb") as part_file:
                bytes_in_part = 0
                while bytes_in_part < max_size:
                    chunk = original_file.read(8192)  # 8KB chunks
                    if not chunk:
                        break
                    part_file.write(chunk)
                    bytes_in_part += len(chunk)
                    bytes_written += len(chunk)

                if bytes_in_part == 0:
                    # 删除空的part文件
                    part_file.close()
                    os.remove(part_filepath)
                    break

            print(f"创建分块文件: {part_filename} ({bytes_in_part} bytes)")
            part_num += 1

            if bytes_written >= file_size:
                break

    # 删除原文件
    os.remove(filepath)
    print(f"完成文件拆分，原文件已删除并添加到.gitignore")
    return True


def check_and_split_large_files(max_size=50 * 1024 * 1024):
    """检查并拆分所有超过指定大小的文件"""
    files = get_git_status_files()
    any_split = False

    for filepath in files:
        if os.path.isfile(filepath):
            file_size = get_file_size(filepath)
            if file_size > max_size:
                if split_large_file(filepath, max_size):
                    any_split = True

    return any_split


def group_files_by_folder(files):
    """按文件夹分组文件"""
    folder_files = {}
    individual_files = []

    for filepath in files:
        if os.path.isfile(filepath):
            parent_dir = str(Path(filepath).parent)
            if parent_dir == ".":
                parent_dir = ""

            if parent_dir not in folder_files:
                folder_files[parent_dir] = []
            folder_files[parent_dir].append(filepath)
        elif os.path.isdir(filepath):
            # 如果是目录，直接添加到文件夹分组
            folder_files[filepath] = [filepath]

    return folder_files


def calculate_batch_size(files_or_folders):
    """计算文件或文件夹列表的总大小"""
    total_size = 0
    for item in files_or_folders:
        if os.path.isfile(item):
            total_size += get_file_size(item)
        elif os.path.isdir(item):
            total_size += get_folder_size(item)
    return total_size


def create_batches(files, max_batch_size=100 * 1024 * 1024):
    """创建分批，每批总大小不超过限制"""
    folder_files = group_files_by_folder(files)
    batches = []
    current_batch = []
    current_batch_size = 0

    # 处理每个文件夹
    for folder, folder_items in folder_files.items():
        folder_total_size = calculate_batch_size(folder_items)

        # 如果整个文件夹大小超过限制，或者当前批次加上整个文件夹会超过限制，则逐个文件处理
        if (
            folder_total_size > max_batch_size
            or current_batch_size + folder_total_size > max_batch_size
        ):
            # 逐个处理文件夹中的文件
            for filepath in folder_items:
                file_size = (
                    get_file_size(filepath)
                    if os.path.isfile(filepath)
                    else get_folder_size(filepath)
                )

                if current_batch_size + file_size > max_batch_size and current_batch:
                    # 开始新批次
                    batches.append(current_batch)
                    current_batch = [filepath]
                    current_batch_size = file_size
                else:
                    current_batch.append(filepath)
                    current_batch_size += file_size
        else:
            # 整个文件夹可以添加到当前批次
            if (
                current_batch_size + folder_total_size > max_batch_size
                and current_batch
            ):
                batches.append(current_batch)
                current_batch = folder_items
                current_batch_size = folder_total_size
            else:
                current_batch.extend(folder_items)
                current_batch_size += folder_total_size

    # 添加最后一个批次
    if current_batch:
        batches.append(current_batch)

    return batches


def git_commit_and_push(commit_info_file):
    """执行git commit和push"""
    commit_cmd = f'git commit -F "{commit_info_file}"'
    push_cmd = "git push"

    print(f"执行命令: {commit_cmd}")
    if os.system(commit_cmd) != 0:
        print("提交失败")
        return False

    print(f"执行命令: {push_cmd}")
    if os.system(push_cmd) != 0:
        print("推送失败")
        return False

    return True


def main():
    if len(sys.argv) != 2:
        print("用法: python script.py commit-info.txt")
        sys.exit(1)

    commit_info_file = sys.argv[1]

    if not os.path.exists(commit_info_file):
        print(f"错误: 文件 {commit_info_file} 不存在")
        sys.exit(1)

    # 检查是否有大文件需要拆分
    print("检查大文件...")
    if check_and_split_large_files(50 * 1024 * 1024):
        print("已拆分大文件，重新扫描文件状态...")
        # 重新获取文件列表，因为拆分后文件状态可能变化
        run_command("git add .")

    # 获取所有需要处理的文件
    files = get_git_status_files()
    if not files:
        print("没有需要提交的文件")
        return

    total_size = calculate_batch_size(files)
    print(f"总文件大小: {total_size / 1024 / 1024:.2f} MB")

    if total_size <= 100 * 1024 * 1024:
        # 一次性提交
        print("文件总大小不超过100M，一次性提交...")
        add_cmd = "git add ."
        print(f"执行命令: {add_cmd}")
        if os.system(add_cmd) == 0:
            git_commit_and_push(commit_info_file)
    else:
        # 分批提交
        print("文件总大小超过100M，分批提交...")
        batches = create_batches(files, 100 * 1024 * 1024)

        for i, batch in enumerate(batches, 1):
            print(f"\n处理第 {i} 批 ({len(batches)} 批总数)")
            batch_size = calculate_batch_size(batch)
            print(f"批次大小: {batch_size / 1024 / 1024:.2f} MB")

            # 添加批次中的文件
            for item in batch:
                add_cmd = f'git add "{item}"'
                print(f"执行命令: {add_cmd}")
                if os.system(add_cmd) != 0:
                    print(f"添加文件失败: {item}")

            # 提交和推送
            if not git_commit_and_push(commit_info_file):
                print(f"第 {i} 批提交失败")
                break
            else:
                print(f"第 {i} 批提交成功")


if __name__ == "__main__":
    main()

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


import os
import sys
import subprocess
import shutil
from pathlib import Path


def get_git_status_files():
    """获取所有已修改和未跟踪的文件"""
    try:
        # 获取已修改和未跟踪的文件
        result = subprocess.run(
            ["git", "status", "--porcelain"], capture_output=True, text=True, check=True
        )
        files = []
        for line in result.stdout.strip().split("\n"):
            if line:
                status = line[:2].strip()
                filepath = line[3:]
                if status in ["M", "A", "??", "AM"]:  # 修改、新增、未跟踪
                    files.append(filepath)
        return files
    except subprocess.CalledProcessError as e:
        print(f"Error getting git status: {e}")
        return []


def get_file_size(filepath):
    """获取文件大小（字节）"""
    try:
        return os.path.getsize(filepath)
    except OSError:
        return 0


def get_total_size(files):
    """计算文件列表的总大小"""
    total = 0
    for filepath in files:
        total += get_file_size(filepath)
        print(filepath, "size:", get_file_size(filepath))
    return total


def find_large_files(files, threshold_mb=50):
    """查找超过阈值的大文件"""
    threshold_bytes = threshold_mb * 1024 * 1024
    large_files = []
    for filepath in files:
        if get_file_size(filepath) > threshold_bytes:
            large_files.append(filepath)
    return large_files


def split_large_file(filepath, chunk_size_mb=10):
    """拆分大文件为小文件"""
    chunk_size = chunk_size_mb * 1024 * 1024
    file_path = Path(filepath)
    parent_dir = file_path.parent
    file_stem = file_path.stem
    file_suffix = file_path.suffix

    # 创建.gitignore文件
    gitignore_path = parent_dir / ".gitignore"
    ignored_patterns = set()

    if gitignore_path.exists():
        with open(gitignore_path, "r") as f:
            ignored_patterns = set(line.strip() for line in f if line.strip())

    # 添加原文件和合并文件到.gitignore
    ignored_patterns.add(file_path.name)
    ignored_patterns.add(f"{file_stem}.merged{file_suffix}")

    with open(gitignore_path, "w") as f:
        for pattern in sorted(ignored_patterns):
            f.write(pattern + "\n")

    # 复制合并工具
    merge_tool = "git-merge-split-files.exe"
    if os.path.exists(merge_tool):
        target_tool = parent_dir / merge_tool
        if not target_tool.exists():
            shutil.copy2(merge_tool, target_tool)

    # 拆分文件
    part_num = 1
    try:
        with open(filepath, "rb") as original_file:
            while True:
                chunk = original_file.read(chunk_size)
                if not chunk:
                    break

                part_filename = (
                    parent_dir / f"{file_stem}.part{part_num:03d}{file_suffix}"
                )
                with open(part_filename, "wb") as part_file:
                    part_file.write(chunk)

                part_num += 1

        print(f"Split {filepath} into {part_num-1} parts")
        return True
    except Exception as e:
        print(f"Error splitting file {filepath}: {e}")
        return False


def group_files_by_directory(files):
    """按目录分组文件"""
    dir_files = {}
    for filepath in files:
        dir_path = os.path.dirname(filepath) or "."  # 当前目录用 '.' 表示
        if dir_path not in dir_files:
            dir_files[dir_path] = []
        dir_files[dir_path].append(filepath)
    return dir_files


def create_batches(files, max_batch_size_mb=100):
    """创建分批，每批不超过最大大小"""
    max_batch_size = max_batch_size_mb * 1024 * 1024
    batches = []
    current_batch = []
    current_size = 0

    # 按目录分组
    dir_files = group_files_by_directory(files)

    # 处理每个目录
    for dir_path, dir_file_list in dir_files.items():
        dir_total_size = get_total_size(dir_file_list)

        if dir_total_size > max_batch_size:
            # 目录总大小超过限制，逐个文件处理
            for filepath in dir_file_list:
                file_size = get_file_size(filepath)

                if current_size + file_size > max_batch_size and current_batch:
                    # 当前批次已满，开始新批次
                    batches.append(current_batch)
                    current_batch = [filepath]
                    current_size = file_size
                else:
                    current_batch.append(filepath)
                    current_size += file_size
        else:
            # 目录总大小未超过限制，尝试整个目录添加到当前批次
            if current_size + dir_total_size > max_batch_size and current_batch:
                # 当前批次已满，开始新批次
                batches.append(current_batch)
                current_batch = dir_file_list
                current_size = dir_total_size
            else:
                current_batch.extend(dir_file_list)
                current_size += dir_total_size

    # 添加最后一个批次
    if current_batch:
        batches.append(current_batch)

    return batches


def git_commit_push(commit_info_file):
    """执行git commit和push"""
    commit_cmd = f'git commit -F "{commit_info_file}"'
    print(f"> {commit_cmd}")
    os.system(commit_cmd)

    push_cmd = "git push"
    print(f"> {push_cmd}")
    os.system(push_cmd)


def main():
    if len(sys.argv) != 2:
        print("Usage: python script.py <commit-info.txt>")
        sys.exit(1)

    commit_info_file = sys.argv[1]

    if not os.path.exists(commit_info_file):
        print(f"Error: Commit info file '{commit_info_file}' does not exist")
        sys.exit(1)

    # 获取所有需要处理的文件
    files = get_git_status_files()
    if not files:
        print("No modified or untracked files found.")
        return

    print(f"Found {len(files)} files to process")

    # 检查大文件并拆分
    large_files = find_large_files(files, 50)
    if large_files:
        print(f"Found {len(large_files)} large files (>50MB) that need splitting:")
        for large_file in large_files:
            print(f"  - {large_file}")

        for large_file in large_files:
            if split_large_file(large_file):
                print(f"Successfully split {large_file}")
            else:
                print(f"Failed to split {large_file}")

        # 重新获取文件列表（拆分后会有新文件）
        files = get_git_status_files()
        print(f"After splitting, found {len(files)} files to process")

    # 计算总大小
    total_size = get_total_size(files)
    total_size_mb = total_size / (1024 * 1024)
    print(f"Total size: {total_size_mb:.2f} MB")

    # 如果总大小不超过100MB，直接提交
    if total_size_mb <= 100:
        print("Total size <= 100MB, committing all files at once")
        add_cmd = "git add ."
        print(f"> {add_cmd}")
        os.system(add_cmd)
        git_commit_push(commit_info_file)
        return

    # 分批处理
    print("Total size > 100MB, processing in batches")
    batches = create_batches(files, 100)

    print(f"Created {len(batches)} batches:")
    for i, batch in enumerate(batches, 1):
        batch_size = get_total_size(batch) / (1024 * 1024)
        print(f"  Batch {i}: {len(batch)} files, {batch_size:.2f} MB")

    # 逐批提交
    for i, batch in enumerate(batches, 1):
        print(f"\n--- Processing batch {i}/{len(batches)} ---")

        for filepath in batch:
            add_cmd = f'git add "{filepath}"'
            print(f"> {add_cmd}")
            os.system(add_cmd)

        git_commit_push(commit_info_file)

        if i < len(batches):
            print("Waiting for next batch...")


if __name__ == "__main__":
    main()

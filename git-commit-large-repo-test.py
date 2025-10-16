# python需求：
# 传递一个参数
# py文件 commit-info.txt
# git add, git commit -F commit-info.txt, git push
# 扫描所有已修改和未跟踪的文件：
# 从最深层的文件夹开始扫描，如果文件夹下所有文件总大小小于等于50M，则加到字典里，键为文件夹相对路径，值为总大小
# 如果文件夹下所有文件总大小大于50M，则单独把所有文件都加入到到字典里，键为文件相对路径，只为文件大小
# 在这个字典的基础之上去git add commit push，这三个命令用os.system去调用
# 如果总大小小于100M，则直接git add -A
# 否则分多个批次进行：
# 每个批次的git add的总大小要小于100M
# 如果某个文件的大小超过100M则跳过它


import os
import sys
import subprocess
from pathlib import Path
from collections import defaultdict


def get_git_status_files():
    """获取所有已修改和未跟踪的文件"""
    try:
        # 获取已修改的文件
        result = subprocess.run(
            ["git", "status", "--porcelain"], capture_output=True, text=True, check=True
        )

        files = []
        for line in result.stdout.strip().split("\n"):
            if line.strip():
                # 状态码在前2位，文件名从第3位开始
                status = line[:2].strip()
                filepath = line[3:].strip()

                # 只关注已修改(M)和未跟踪(??)的文件
                if status in ["M", "A", "??"]:
                    files.append(filepath)

        return files
    except subprocess.CalledProcessError as e:
        print(f"获取Git状态失败: {e}")
        return []


def calculate_size(files):
    """计算文件大小，返回文件路径和大小的字典"""
    file_sizes = {}
    for filepath in files:
        try:
            if os.path.exists(filepath):
                size = os.path.getsize(filepath)
                file_sizes[filepath] = size
        except OSError as e:
            print(f"无法获取文件大小 {filepath}: {e}")

    return file_sizes


def organize_files_by_folder(file_sizes):
    """按文件夹组织文件，根据大小决定是否合并文件夹"""
    folder_files = defaultdict(list)
    folder_sizes = {}

    # 首先按文件夹分组
    for filepath, size in file_sizes.items():
        folder = os.path.dirname(filepath) or "."  # 根目录用'.'表示
        folder_files[folder].append((filepath, size))

    # 计算每个文件夹的总大小
    for folder, files in folder_files.items():
        total_size = sum(size for _, size in files)
        folder_sizes[folder] = total_size

    # 处理嵌套文件夹，从最深层的开始
    sorted_folders = sorted(
        folder_files.keys(), key=lambda x: x.count(os.path.sep), reverse=True
    )

    result = {}
    processed_folders = set()

    for folder in sorted_folders:
        # 如果这个文件夹已经被处理（作为子文件夹被父文件夹包含），则跳过
        if any(folder.startswith(p + os.path.sep) for p in processed_folders):
            continue

        total_size = folder_sizes[folder]

        if total_size <= 50 * 1024 * 1024:  # 50MB
            # 文件夹总大小 <= 50MB，添加整个文件夹
            result[folder] = total_size
            processed_folders.add(folder)
        else:
            # 文件夹总大小 > 50MB，单独添加每个文件
            for filepath, size in folder_files[folder]:
                if size <= 100 * 1024 * 1024:  # 跳过超过100MB的单个文件
                    result[filepath] = size
            processed_folders.add(folder)

    # 处理根目录的文件
    root_files = folder_files.get(".", [])
    root_total = sum(size for _, size in root_files)

    if root_total <= 50 * 1024 * 1024 and "." not in processed_folders:
        result["."] = root_total
    elif "." not in processed_folders:
        for filepath, size in root_files:
            if size <= 100 * 1024 * 1024:
                result[filepath] = size

    return result


def create_batches(file_dict, max_batch_size=100 * 1024 * 1024):
    """创建批次，每个批次总大小不超过max_batch_size"""
    if not file_dict:
        return []

    total_size = sum(file_dict.values())

    # 如果总大小小于100MB，直接作为一个批次
    if total_size <= max_batch_size:
        return [list(file_dict.keys())]

    # 否则需要分批
    batches = []
    current_batch = []
    current_size = 0

    # 按文件大小降序排序，优先处理大文件
    sorted_items = sorted(file_dict.items(), key=lambda x: x[1], reverse=True)

    for path, size in sorted_items:
        # 如果单个文件就超过批次大小，跳过
        if size > max_batch_size:
            print(f"跳过过大文件: {path} ({size / 1024 / 1024:.2f}MB)")
            continue

        if current_size + size <= max_batch_size:
            current_batch.append(path)
            current_size += size
        else:
            # 当前批次已满，开始新批次
            if current_batch:
                batches.append(current_batch)
            current_batch = [path]
            current_size = size

    # 添加最后一个批次
    if current_batch:
        batches.append(current_batch)

    return batches


def git_operations(batches, commit_file):
    """执行Git操作"""
    if not batches:
        print("没有需要提交的文件")
        return

    print(f"总共 {len(batches)} 个批次需要提交")

    for i, batch in enumerate(batches, 1):
        print(f"\n正在处理第 {i}/{len(batches)} 个批次...")

        # 添加文件到Git
        for filepath in batch:
            try:
                if os.path.isdir(filepath) and filepath != ".":
                    # 如果是文件夹，添加整个文件夹
                    subprocess.run(["git", "add", filepath], check=True)
                    print(f"添加文件夹: {filepath}")
                else:
                    # 单个文件或根目录
                    subprocess.run(["git", "add", filepath], check=True)
                    print(f"添加文件: {filepath}")
            except subprocess.CalledProcessError as e:
                print(f"添加失败 {filepath}: {e}")
                continue

        # 提交
        try:
            subprocess.run(["git", "commit", "-F", commit_file], check=True)
            print("提交成功")
        except subprocess.CalledProcessError as e:
            print(f"提交失败: {e}")
            continue

        # 推送
        try:
            subprocess.run(["git", "push"], check=True)
            print("推送成功")
        except subprocess.CalledProcessError as e:
            print(f"推送失败: {e}")
            continue


def main():
    if len(sys.argv) != 2:
        print("用法: python script.py commit-info.txt")
        sys.exit(1)

    commit_file = sys.argv[1]

    if not os.path.exists(commit_file):
        print(f"提交信息文件不存在: {commit_file}")
        sys.exit(1)

    print("扫描Git状态...")
    git_files = get_git_status_files()

    if not git_files:
        print("没有找到需要提交的文件")
        return

    print(f"找到 {len(git_files)} 个需要处理的文件")

    print("计算文件大小...")
    file_sizes = calculate_size(git_files)

    print("按文件夹组织文件...")
    organized_files = organize_files_by_folder(file_sizes)

    print("创建提交批次...")
    batches = create_batches(organized_files)

    print("执行Git操作...")
    git_operations(batches, commit_file)

    print("\n所有操作完成!")


if __name__ == "__main__":
    main()

import os
import sys
import subprocess
import shutil
from collections import defaultdict

commit_info_file = ""


def split_large_file(file_path, chunk_size=50 * 1024 * 1024):
    """拆分大文件为多个小文件"""
    try:
        file_dir = os.path.dirname(file_path)
        file_name = os.path.basename(file_path)
        base_name, file_ext = os.path.splitext(file_name)
        split_dir = os.path.join(file_dir, f"{base_name}{file_ext}-split")
        os.makedirs(split_dir, exist_ok=True)
        with open(file_path, "rb") as f:
            chunk_num = 1
            while True:
                chunk = f.read(chunk_size)
                if not chunk:
                    break
                chunk_file = os.path.join(
                    split_dir, f"{base_name}{file_ext}-part{chunk_num:04d}"
                )
                with open(chunk_file, "wb") as chunk_f:
                    chunk_f.write(chunk)
                chunk_num += 1
        print(f"✅ Split {file_path} into {chunk_num-1} parts")
        return True
    except Exception as e:
        print(f"❌ Error splitting file {file_path}: {e}")
        return False


def find_large_files_in_directory(directory, min_size_mb=50):
    """递归查找目录中所有大于指定大小的文件"""
    large_files = []
    try:
        for root, dirs, files in os.walk(directory):
            for file in files:
                file_path = os.path.join(root, file)
                try:
                    size_mb = get_file_size_mb(file_path)
                    if size_mb > min_size_mb:
                        large_files.append((file_path, size_mb))
                except OSError:
                    continue
    except Exception as e:
        print(f"❌ Error scanning directory {directory}: {e}")
    return large_files


def process_large_untracked_files():
    """处理所有超过50MB的未跟踪文件和文件夹中的大文件"""
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
        large_files = []
        processed_dirs = set()
        for line in lines:
            if line.strip() and line.startswith("??"):
                item_path = line[3:].strip()
                if " -> " in item_path:
                    item_path = item_path.split(" -> ")[1]
                if item_path.startswith('"') and item_path.endswith('"'):
                    item_path = item_path[1:-1]
                if os.path.isfile(item_path):
                    size_mb = get_file_size_mb(item_path)
                    if size_mb > 50:
                        large_files.append((item_path, size_mb))
                elif os.path.isdir(item_path):
                    dir_path = os.path.normpath(item_path)
                    if dir_path not in processed_dirs:
                        processed_dirs.add(dir_path)
                        dir_large_files = find_large_files_in_directory(dir_path)
                        large_files.extend(dir_large_files)
        if not large_files:
            return
        print(f"🔍 Found {len(large_files)} large untracked files (>50MB):")
        for file_path, size_mb in large_files:
            print(f"   {file_path}: {size_mb:.2f} MB")
        for file_path, size_mb in large_files:
            process_single_large_file(file_path)
    except Exception as e:
        print(f"❌ Error processing large untracked files: {e}")


def process_single_large_file(file_path):
    """处理单个大文件"""
    try:
        file_dir = os.path.dirname(file_path)
        file_name = os.path.basename(file_path)
        base_name, file_ext = os.path.splitext(file_name)
        print(f"\n📦 Processing large file: {file_path}")
        gitignore_path = os.path.join(file_dir, ".gitignore")
        gitignore_content = []
        if os.path.exists(gitignore_path):
            with open(gitignore_path, "r", encoding="utf-8") as f:
                gitignore_content = f.read().splitlines()
        patterns_to_add = [file_name, f"{base_name}{file_ext}.merged"]
        for pattern in patterns_to_add:
            if pattern not in gitignore_content:
                gitignore_content.append(pattern)
        with open(gitignore_path, "w", encoding="utf-8") as f:
            f.write("\n".join(gitignore_content))
        print(f"✅ Updated .gitignore in {file_dir}")
        exe_name = "git-merge-split-files.exe"
        target_exe_path = os.path.join(file_dir, exe_name)
        current_dir = os.path.dirname(os.path.abspath(__file__))
        source_exe_path = os.path.join(current_dir, exe_name)
        if not os.path.exists(target_exe_path) and os.path.exists(source_exe_path):
            shutil.copy2(source_exe_path, target_exe_path)
            print(f"✅ Copied {exe_name} to {file_dir}")
        elif not os.path.exists(source_exe_path):
            print(f"⚠️  {exe_name} not found in script directory")
        if split_large_file(file_path):
            print(f"✅ Successfully processed {file_path}")
        else:
            print(f"❌ Failed to process {file_path}")
    except Exception as e:
        print(f"❌ Error processing large file {file_path}: {e}")


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
    dir_files_map = defaultdict(list)
    for file_path, size_mb in all_files:
        if dir_path := os.path.dirname(file_path):
            dir_files_map[dir_path].append((file_path, size_mb))
    sorted_dirs = sorted(
        dir_files_map.keys(), key=lambda x: x.count(os.sep), reverse=True
    )
    result_dict, processed_files = {}, set()
    for dir_path in sorted_dirs:
        current_files = [
            (f, s) for f, s in dir_files_map[dir_path] if f not in processed_files
        ]
        if not current_files:
            continue
        total_size = sum(size for _, size in current_files)
        if total_size <= 50:
            result_dict[dir_path] = total_size
            for file_path, _ in current_files:
                processed_files.add(file_path)
        else:
            for file_path, size_mb in current_files:
                if file_path not in processed_files:
                    result_dict[file_path] = size_mb
                    processed_files.add(file_path)
    for file_path, size_mb in all_files:
        if not os.path.dirname(file_path) and file_path not in processed_files:
            result_dict[file_path] = size_mb
    return result_dict, deleted_files


def execute_git_commands(files_dict, deleted_files):
    """执行git命令"""
    if not files_dict and not deleted_files:
        print("📭 No files to commit")
        return False
    total_size = sum(files_dict.values()) if files_dict else 0
    print(f"📊 Total size: {total_size:.2f} MB")
    if deleted_files:
        print(f"🗑️  Deleted files: {len(deleted_files)}")
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
    current_batch, current_size, committed_size, batch_num = [], 0, 0, 0
    for path, size in files_dict.items():
        if size > 50:
            print(f"⏭️  Skipping large file: {path} ({size:.2f} MB)")
            continue
        if current_size + size <= 100:
            current_batch.append(path)
            current_size += size
        else:
            if current_batch:
                batch_num += 1
                print(f"\n🟢 Starting Batch {batch_num}/{total_batches}")
                print(
                    f"📈 Progress: {committed_size:.2f}/{total_size:.2f} MB ({committed_size/total_size*100:.1f}%)"
                )
                commit_batch(current_batch, current_size, batch_num, total_batches)
                committed_size += current_size
                print(f"✅ Completed Batch {batch_num}/{total_batches}")
                print(
                    f"📈 Progress: {committed_size:.2f}/{total_size:.2f} MB ({committed_size/total_size*100:.1f}%)"
                )
                current_batch, current_size = [], 0
            current_batch.append(path)
            current_size = size
    if current_batch:
        batch_num += 1
        print(f"\n🟢 Starting Batch {batch_num}/{total_batches}")
        print(
            f"📈 Progress: {committed_size:.2f}/{total_size:.2f} MB ({committed_size/total_size*100:.1f}%)"
        )
        commit_batch(current_batch, current_size, batch_num, total_batches)
        committed_size += current_size
        print(f"✅ Completed Batch {batch_num}/{total_batches}")
        print(
            f"📈 Progress: {committed_size:.2f}/{total_size:.2f} MB ({committed_size/total_size*100:.1f}%)"
        )
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
    successful_adds = 0
    for file_path in valid_paths:
        normalized_path = os.path.normpath(file_path)
        if not os.path.exists(normalized_path):
            print(f"⚠️  Path no longer exists: {normalized_path}")
            continue
        success = run_git_commands([f'git add "{normalized_path}"'])
        if success:
            successful_adds += 1
        else:
            print(f"❌ git add failed: {normalized_path}")
    if successful_adds == 0:
        print("⏭️  No files added, skipping commit")
        return False
    print(f"✅ Added {successful_adds}/{len(valid_paths)} files")
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
    print("🔍 Checking for large untracked files...")
    process_large_untracked_files()
    files_dict, deleted_files = scan_and_categorize_files()
    if not files_dict and not deleted_files:
        print("📭 No files to process")
        return
    success = execute_git_commands(files_dict, deleted_files)
    if success:
        print("\n🎉 All operations completed successfully!")
    else:
        print("\n❌ Some operations failed")
        sys.exit(1)


if __name__ == "__main__":
    main()

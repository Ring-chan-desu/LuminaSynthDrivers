# 1. 定义你要链接到的源头“指定目录”（请在下方双引号中填入你的绝对路径）
# 例如: $sourceDir = "D:\MyTargetFolder"
$sourceDir = "..\..\LuminaSynthDrivers"

# 检查你是否填写了指定目录路径
if ([string]::IsNullOrWhiteSpace($sourceDir)) {
    Write-Warning "提示：脚本中的 `$sourceDir 变量目前为空，请先编辑本脚本填入源文件夹路径后再运行。"
    Read-Host "按任意键退出..."
    exit
}

# 检查该指定目录是否存在
if (-not (Test-Path -Path $sourceDir -PathType Container)) {
    Write-Error "错误：指定的源头目录不存在，请检查路径：$sourceDir"
    Read-Host "按任意键退出..."
    exit
}

# 2. 让用户输入要在哪里创建软链接
$targetParentDir = Read-Host "请输入要在哪个目录下创建软链接"

# 检查用户输入的目录是否存在
if (-not (Test-Path -Path $targetParentDir -PathType Container)) {
    Write-Error "错误：输入的目录不存在，请检查路径是否正确！"
    Read-Host "按任意键退出..."
    exit
}

# 获取源文件夹的名字，用于在目标位置生成同名的软链接文件夹
$dirName = Split-Path -Leaf $sourceDir
$linkPath = Join-Path -Path $targetParentDir -ChildPath $dirName

# 3. 创建目录软链接
try {
    # -ItemType Junction 在 Windows 下创建目录联接（不需要管理员权限）
    # 如果你想用标准的符号链接，可以把 Junction 改为 SymbolicLink（但需要管理员权限）
    New-Item -ItemType Junction -Path $linkPath -Value $sourceDir -Force
    Write-Host "成功：已在目录 [$targetParentDir] 下创建了指向 [$sourceDir] 的目录软链接！" -ForegroundColor Green
}
catch {
    Write-Error "创建目录软链接失败：$_"
}

# 4. 留下提示信息并等待用户按任意键退出
Write-Host ""
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host " 软链接脚本执行完毕。请检查上方日志。 " -ForegroundColor Cyan
Write-Host "=============================================" -ForegroundColor Cyan
Read-Host "按任意键退出..."
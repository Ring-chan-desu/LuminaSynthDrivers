# =============================================================================
# 0. 语言选择代码 (Language Selection Core)
# =============================================================================
Clear-Host
Write-Host "=============================================" -ForegroundColor Yellow
Write-Host " Please Select Your Language / 请选择语言 "
Write-Host " [1] English"
Write-Host " [2] 简体中文 (Simplified Chinese)"
Write-Host " [3] 日本語 (Japanese)"
Write-Host "=============================================" -ForegroundColor Yellow

$langChoice = Read-Host "Choice / 选择 / 選択 (1-3)"

# 初始化多语言提示文本字典 (Locales Dictionary)
$msg = @{}

switch ($langChoice) {
    "2" {
        # 简体中文
        $msg.WarnEmpty   = "提示：脚本中的 `$sourceDir 变量目前为空，请先编辑本脚本填入源文件夹路径后再运行。"
        $msg.ErrSrcExist = "错误：指定的源头目录不存在，请检查路径：{0}"
        $msg.PromptTarget= "请输入要在哪个目录下创建软链接"
        $msg.ErrTgtExist = "错误：输入的目录不存在，请检查路径是否正确！"
        $msg.Success     = "成功：已在目录 [{0}] 下创建了指向 [{1}] 的目录软链接！"
        $msg.ErrFail     = "创建目录软链接失败：{0}"
        $msg.Footer      = " 软链接脚本执行完毕。请检查上方日志。 "
        $msg.ExitKey     = "按任意键退出..."
    }
    "3" {
        # 日本語
        $msg.WarnEmpty   = "ヒント：スクリプト内の `$sourceDir 変数が空です。このスクリプトを編集してソースフォルダのパスを入力してから実行してください。"
        $msg.ErrSrcExist = "エラー：指定されたソースディレクトリが存在しません。パスを確認してください：{0}"
        $msg.PromptTarget= "シンボリックリンクを作成するターゲットディレクトリを入力してください"
        $msg.ErrTgtExist = "エラー：入力されたディレクトリが存在しません。パスが正しいか確認してください！"
        $msg.Success     = "成功：ディレクトリ [{0}] 内に [{1}] を指すディレクトリシンボリックリンクを作成しました！"
        $msg.ErrFail     = "ディレクトリシンボリックリンクの作成に失敗しました：{0}"
        $msg.Footer      = " シンボリックリンクスクリプトの実行が完了しました。上記のログを確認してください。 "
        $msg.ExitKey     = "何かキーを押して終了します..."
    }
    default {
        # Default: English
        $msg.WarnEmpty   = "Warning: The `$sourceDir variable in the script is currently empty. Please edit this script to fill in the source folder path before running."
        $msg.ErrSrcExist = "Error: The specified source directory does not exist. Please check the path: {0}"
        $msg.PromptTarget= "Please enter the target directory where the symbolic link should be created"
        $msg.ErrTgtExist = "Error: The entered directory does not exist. Please check if the path is correct!"
        $msg.Success     = "Success: Created a directory symbolic link pointing to [{1}] under the directory [{0}]!"
        $msg.ErrFail     = "Failed to create directory symbolic link: {0}"
        $msg.Footer      = " Symbolic link script execution complete. Please check the logs above. "
        $msg.ExitKey     = "Press any key to exit..."
    }
}


# =============================================================================
# 1. 定义你要链接到的源头“指定目录”
# =============================================================================
$sourceDir = "..\..\LuminaSynthDrivers"

# 检查你是否填写了指定目录路径
if ([string]::IsNullOrWhiteSpace($sourceDir)) {
    Write-Warning $msg.WarnEmpty
    Read-Host $msg.ExitKey
    exit
}

# 检查该指定目录是否存在
if (-not (Test-Path -Path $sourceDir -PathType Container)) {
    Write-Error ($msg.ErrSrcExist -f $sourceDir)
    Read-Host $msg.ExitKey
    exit
}


# =============================================================================
# 2. 让用户输入要在哪里创建软链接
# =============================================================================
$targetParentDir = Read-Host $msg.PromptTarget

# 检查用户输入的目录是否存在
if (-not (Test-Path -Path $targetParentDir -PathType Container)) {
    Write-Error $msg.ErrTgtExist
    Read-Host $msg.ExitKey
    exit
}

# 获取源文件夹的名字，用于在目标位置生成同名的软链接文件夹
$dirName = Split-Path -Leaf $sourceDir
$linkPath = Join-Path -Path $targetParentDir -ChildPath $dirName


# =============================================================================
# 3. 创建目录软链接
# =============================================================================
try {
    # -ItemType Junction 在 Windows 下创建目录联接（不需要管理员权限）
    # 如果你想用标准的符号链接，可以把 Junction 改为 SymbolicLink（但需要管理员权限）
    New-Item -ItemType Junction -Path $linkPath -Value $sourceDir -Force
    Write-Host ($msg.Success -f $targetParentDir, $sourceDir) -ForegroundColor Green
}
catch {
    Write-Error ($msg.ErrFail -f $_)
}


# =============================================================================
# 4. 留下提示信息并等待用户按任意键退出
# =============================================================================
Write-Host ""
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host $msg.Footer -ForegroundColor Cyan
Write-Host "=============================================" -ForegroundColor Cyan
Read-Host $msg.ExitKey
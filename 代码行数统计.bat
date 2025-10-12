@echo off
setlocal enabledelayedexpansion

rem -----------------------
rem CountLines.bat [path]
rem 如果不传 path 则使用当前目录
rem 统计扩展名：*.cpp *.h *.css *.ui
rem 排除路径中包含 "build" 的文件夹（大小写不敏感）
rem 请用 NOTEPAD 另存为 ANSI 编码（Windows 中文系统推荐）
rem -----------------------

if "%~1"=="" (
  set "SEARCH_PATH=%cd%"
) else (
  set "SEARCH_PATH=%~1"
)

rem 临时文件
set "TMP_PERFILE=%TEMP%\count_lines_perfile_%RANDOM%.txt"
if exist "%TMP_PERFILE%" del /f /q "%TMP_PERFILE%" >nul 2>&1

rem 初始化
set /a TOTAL_FILES=0
set /a TOTAL_LINES=0

set /a FILES_cpp=0
set /a LINES_cpp=0
set /a FILES_h=0
set /a LINES_h=0
set /a FILES_css=0
set /a LINES_css=0
set /a FILES_ui=0
set /a LINES_ui=0

echo Scanning: %SEARCH_PATH%
echo.

rem 遍历目标文件（递归）
for /R "%SEARCH_PATH%" %%F in (*.cpp *.h *.css *.ui) do (
    set "FULLPATH=%%~fF"

    rem 如果路径中包含 "build" 则跳过（大小写不敏感）
    echo !FULLPATH! | findstr /i /c:"\build\" >nul
    if errorlevel 1 (
        rem 未找到 "build"，处理该文件

        rem 统计行数（包括空行）
        for /f "usebackq delims=" %%A in (`type "%%~fF" ^| find /v /c ""`) do set "COUNT=%%A"
        if not defined COUNT set "COUNT=0"

        rem 更新汇总
        set /a TOTAL_FILES+=1
        set /a TOTAL_LINES+=COUNT

        rem 扩展名（去掉点）
        set "ext=%%~xF"
        set "ext=!ext:~1!"

        if /I "!ext!"=="cpp" (
            set /a FILES_cpp+=1
            set /a LINES_cpp+=COUNT
        ) else if /I "!ext!"=="h" (
            set /a FILES_h+=1
            set /a LINES_h+=COUNT
        ) else if /I "!ext!"=="css" (
            set /a FILES_css+=1
            set /a LINES_css+=COUNT
        ) else if /I "!ext!"=="ui" (
            set /a FILES_ui+=1
            set /a LINES_ui+=COUNT
        )

        rem 用固定宽度前缀（12 位）便于字典排序
        set "pad=000000000000!COUNT!"
        set "padded=!pad:~-12!"
        >>"%TMP_PERFILE%" echo !padded!^|%%~fF

        rem 清理临时变量
        set "COUNT="
    ) else (
        rem 找到 "build"，跳过该文件
        rem echo Skipped: %%~fF
    )
)

rem 输出按扩展名统计（注意：括号已转义）
echo === By extension ===
if %FILES_cpp% NEQ 0 echo .cpp  Files: %FILES_cpp%   Lines: %LINES_cpp%
if %FILES_h%   NEQ 0 echo .h    Files: %FILES_h%   Lines: %LINES_h%
if %FILES_css% NEQ 0 echo .css  Files: %FILES_css%   Lines: %LINES_css%
if %FILES_ui%  NEQ 0 echo .ui   Files: %FILES_ui%   Lines: %LINES_ui%

echo.
echo Total files: %TOTAL_FILES%
echo Total lines: %TOTAL_LINES%
echo.

rem 输出按行数排序的前 10 个文件（从大到小）
if exist "%TMP_PERFILE%" (
    echo === Top 10 files by lines ===
    set /a CNT=0
    for /f "usebackq delims=" %%L in ('sort /R "%TMP_PERFILE%"') do (
        set /a CNT+=1
        if !CNT! GTR 10 goto :TOP_DONE

        set "LINE=%%L"
        rem padded count 是前12個字符，'|' 在第13位
        set "COUNTSTR=!LINE:~0,12!"
        set "FILEPATH=!LINE:~13!"

        rem 调用子例程去掉前导零（子例程在文件末）
        call :StripZeros "%COUNTSTR%"
        rem STRIPPED 由子例程设置
        echo !STRIPPED!    !FILEPATH!
    )
    :TOP_DONE
    del /f /q "%TMP_PERFILE%" >nul 2>&1
) else (
    rem 这里要转义圆括号，避免在括号块内出现未转义的 ( )
    echo No matching files ^( *.cpp, *.h, *.css, *.ui ^) found.
)

endlocal

rem ---------- 子例程：去掉字符串前导零并通过 STRIPPED 返回 ----------
:StripZeros
rem 参数：%~1 = 待处理字符串（可能含前导零）
setlocal enabledelayedexpansion
set "s=%~1"
:SZ_LOOP
if "!s:~0,1!"=="0" (
    set "s=!s:~1!"
    goto SZ_LOOP
)
if "!s!"=="" set "s=0"
endlocal & set "STRIPPED=%s%"
goto :eof

@echo off
title LAYNCORP // WINDOWS 11 LTSC NETBOOT SETUP
color 0B
cls
echo ========================================================================
echo   LAYNCORP // B75CR501 - WINDOWS 11 ENTERPRISE LTSC 2024 (PT-BR)
echo ========================================================================
echo.
echo [1/3] Aplicando bypass de TPM 2.0 / Secure Boot / RAM / CPU...
reg add "HKLM\SYSTEM\Setup\LabConfig" /v BypassTPMCheck /t REG_DWORD /d 1 /f >nul
reg add "HKLM\SYSTEM\Setup\LabConfig" /v BypassSecureBootCheck /t REG_DWORD /d 1 /f >nul
reg add "HKLM\SYSTEM\Setup\LabConfig" /v BypassRAMCheck /t REG_DWORD /d 1 /f >nul
reg add "HKLM\SYSTEM\Setup\LabConfig" /v BypassStorageCheck /t REG_DWORD /d 1 /f >nul
reg add "HKLM\SYSTEM\Setup\LabConfig" /v BypassCPUCheck /t REG_DWORD /d 1 /f >nul
reg add "HKLM\SYSTEM\Setup\MoSetup" /v AllowUpgradesWithUnsupportedTPMOrCPU /t REG_DWORD /d 1 /f >nul
echo       Bypass aplicado com sucesso.
echo.
echo [2/3] Inicializando rede WinPE (wpeinit)...
wpeinit
echo.
if exist X:\Windows\System32\curl.exe (
    for /f "tokens=1,2" %%a in ('X:\Windows\System32\curl.exe --version ^| find /i "curl"') do echo [3/3] Downloader pronto: %%a %%b
) else (
    echo [!] ERRO: curl.exe nao encontrado no WinPE!
)
echo.

set "WIN_URL=https://laynos.app/win11/sources/install.wim"
if exist X:\Windows\System32\win_url.txt (
    set /p WIN_URL=<X:\Windows\System32\win_url.txt
)

:menu
echo ------------------------------------------------------------------------
echo  URL da imagem: %WIN_URL%
echo ------------------------------------------------------------------------
(
echo list disk
echo list volume
echo exit
) > X:\Windows\Temp\dp_list.txt
for /f "delims=" %%L in ('diskpart /s X:\Windows\Temp\dp_list.txt ^| find /i "Disco "') do echo  %%L
for /f "delims=" %%L in ('diskpart /s X:\Windows\Temp\dp_list.txt ^| find /i "Disk "') do echo  %%L
echo.
for /f "delims=" %%L in ('diskpart /s X:\Windows\Temp\dp_list.txt ^| find /i "Volume "') do echo  %%L
echo.
echo  Escolha uma opcao:
echo    [1] Instalar limpo em um disco (cria EFI + Windows + cache temp W:)
echo    [2] Baixar install.wim em uma letra existente (ex: D:) e abrir Setup
echo    [3] Abrir prompt de comando (CMD) / testar rede
echo    [4] Reiniciar computador
echo.
set "OPT="
set /p "OPT=Opcao [1-4]: "
if "%OPT%"=="1" goto clean_disk
if "%OPT%"=="2" goto existing_vol
if "%OPT%"=="3" goto open_cmd
if "%OPT%"=="4" wpeutil reboot
goto menu

:clean_disk
echo.
set "DNUM="
set /p "DNUM=Digite o numero do DISCO alvo (ex: 0): "
if "%DNUM%"=="" goto menu
echo.
echo  [!] ATENCAO: TODOS OS DADOS DO DISCO %DNUM% SERAO APAGADOS!
set "CONF="
set /p "CONF=Digite SIM para confirmar a formatacao do Disco %DNUM%: "
if /i not "%CONF%"=="SIM" goto menu

(
echo select disk %DNUM%
echo clean
echo convert gpt
echo create partition efi size=260
echo format quick fs=fat32 label="EFI"
echo assign letter=S
echo create partition msr size=16
echo create partition primary
echo shrink desired=5120
echo format quick fs=ntfs label="Windows"
echo assign letter=C
echo create partition primary
echo format quick fs=ntfs label="LAYNWIN"
echo assign letter=W
echo exit
) > X:\Windows\Temp\dp_clean.txt

echo [+] Particionando disco %DNUM% em GPT UEFI...
diskpart /s X:\Windows\Temp\dp_clean.txt
if errorlevel 1 (
    echo [!] Falha ao particionar o disco %DNUM%.
    pause
    goto menu
)
set "TARGET_WIM=W:\install.wim"
goto download_and_setup

:existing_vol
echo.
set "DRV="
set /p "DRV=Digite a letra da particao com >= 5 GB livres (ex: D): "
if "%DRV%"=="" goto menu
set "DRV=%DRV:~0,1%"
set "TARGET_WIM=%DRV%:\install.wim"

:download_and_setup
echo.
echo [+] Baixando install.wim (~3.90 GB) do Cloudflare R2 para %TARGET_WIM% ...
X:\Windows\System32\curl.exe -fL --progress-bar --retry 3 "%WIN_URL%" -o "%TARGET_WIM%"
if errorlevel 1 (
    echo.
    echo [!] Erro ao baixar %WIN_URL%. Verifique DHCP/DNS/URL publica do R2.
    pause
    goto menu
)
echo.
echo [+] Download concluido! Iniciando instalador oficial Windows 11 (PT-BR)...
X:\sources\setup.exe /installfrom:"%TARGET_WIM%"
goto menu

:open_cmd
echo.
echo  Digite 'exit' para voltar ao menu ou 'X:\sources\setup.exe' para abrir o Setup.
cmd
goto menu

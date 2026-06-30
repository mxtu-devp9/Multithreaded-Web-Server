@echo off
set SERVER_IP=%1
if "%SERVER_IP%"=="" set SERVER_IP=127.0.0.1
set COUNT=1

echo Continuous Auto Request Demo Started
echo Server IP: %SERVER_IP%
echo Press CTRL + C to stop

:loop
echo Round %COUNT%
start /B curl -s http://%SERVER_IP%:8080/data?name=Rishabh^&type=Normal
start /B curl -s http://%SERVER_IP%:8080/client-status?name=Avi^&type=Normal
start /B curl -s http://%SERVER_IP%:8080/file?name=Abhi^&type=Normal

timeout /t 2 /nobreak >nul
set /A MOD=%COUNT% %% 5
if %MOD%==0 (
  echo Sending priority request
  curl -s http://%SERVER_IP%:8080/pause?name=Inder^&type=Priority >nul
)
set /A COUNT=%COUNT%+1
goto loop

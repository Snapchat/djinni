@ 2>/dev/null # 2>nul & echo off & goto BOF
:
exec java -jar $JAVA_OPTS "$0" "$@"
exit

:BOF
@echo off
java -jar %JAVA_OPTS% "%~dpnx0" %*
exit /B %errorlevel%

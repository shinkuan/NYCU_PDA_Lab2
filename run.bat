@echo off
REM =============================================================================
REM Script Name: run_lab2.bat
REM Description:
REM   - Executes Lab2.exe for predefined test cases.
REM   - If a command-line argument is provided (e.g., run_lab2.bat another_testcase),
REM     the script will execute Lab2.exe only for the specified test case.
REM   - If no argument is provided, the script will iterate through all predefined
REM     test cases and execute Lab2.exe for each.
REM =============================================================================

REM Enable delayed variable expansion
SETLOCAL ENABLEDELAYEDEXPANSION

REM Define a list of test cases
SET "elements=0 ami33 ami49 apte hp vda317b xerox"
GOTO main

REM Function: Execute Lab2.exe for a given test case
:run_lab2
SET "testcase=%~1"

REM Define the input and output file paths
SET "input_block=.\input\%testcase%\%testcase%.block"
SET "input_nets=.\input\%testcase%\%testcase%.nets"
SET "output_rpt=.\output\%testcase%.rpt"

REM Check if the input files exist
IF NOT EXIST "%input_block%" (
    ECHO Input file "%input_block%" does not exist. Skipping "%testcase%".
    GOTO :EOF
)

IF NOT EXIST "%input_nets%" (
    ECHO Input file "%input_nets%" does not exist. Skipping "%testcase%".
    GOTO :EOF
)

REM Execute Lab2.exe with the specified arguments
ECHO Executing command: Lab2.exe 0.5 "%input_block%" "%input_nets%" "%output_rpt%"
Lab2.exe 0.5 "%input_block%" "%input_nets%" "%output_rpt%"

REM Check if Lab2.exe executed successfully
IF ERRORLEVEL 1 (
    ECHO Error occurred while processing "%testcase%".
) ELSE (
    ECHO Successfully processed "%testcase%".
)

GOTO :EOF

REM =============================================================================
REM Main Script Execution
REM =============================================================================
:main

REM Check the number of command-line arguments
IF "%~2" NEQ "" (
    ECHO Usage: %~nx0 [testcase]
    ECHO   testcase - Optional. Specify a single test case to execute.
    EXIT /B 1
)

IF NOT "%~1"=="" (
    REM If an argument is provided, execute only for that test case
    CALL :run_lab2 "%~1"
    EXIT /B
)

REM If no arguments are provided, iterate through all test cases
FOR %%n IN (%elements%) DO (
    CALL :run_lab2 "%%n"
)

ENDLOCAL
EXIT /B 0

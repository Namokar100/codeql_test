@echo off
echo Running CodeQL analysis...
"C:\Users\Namokar\Downloads\codeql-win64\codeql\codeql.exe" database create --threads=0 --language=cpp ^
  "C:\Users\Namokar\Desktop\HPE project\codeql_sw\database" ^
  -s "C:\Users\Namokar\Desktop\HPE project\codeql_test" ^
  --command="build.cmd" ^
  --overwrite
echo CodeQL analysis completed. 
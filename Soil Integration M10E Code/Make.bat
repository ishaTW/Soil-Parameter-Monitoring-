@ECHO OFF
IF "%1" == "" (
	ECHO More parameter is required. 
	ECHO  - If you're using GCC compiler, "make clean" may clean the compiling, and "make new" may start to compile.
	ECHO  - If you're using RVCT compiler, to input "make rvct help" can know the usage.
) ELSE (
	IF /i "%1" == "rvct" (
		IF "%2" == "" (
			ECHO More parameter is required.
		) ELSE (			
::		    ECHO Start to rvct_make.bat
		    CALL make\rvct\rvct_make.bat %2
		)
	) ELSE (
		CALL make\gcc\gcc_make.bat %1
	)
)

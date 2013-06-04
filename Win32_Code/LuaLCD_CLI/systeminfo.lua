require("amdcpu")
require("nvidiagpu")
require("perfcounter")


function rjust( a, b ) 

	text = tostring( a )
	len = string.len ( text )
	
	while len < b do
		text = " " .. text
		len = len + 1
	end

return text
end


function cputemp() 

		ctemp = amdcpu.getCPUTemp( )
		ftemp = ctemp * 9/5  + 32
		
	return string.format ( "%5.1fC (%5.1fF)", ctemp, ftemp)
end

function gputemp() 

		ctemp = nvidiagpu.getGPUTemp( )
		ftemp = ctemp * 9/5  + 32
		
	return string.format ( "%5.1fC (%5.1fF)", ctemp, ftemp)
end

function gpuUseage() 

		utemp = nvidiagpu.getGPUUsage() 
				
	return string.format ( "%3i", utemp)
end

function gpuClocks() 

		cctemp = nvidiagpu.getGPUCoreClock() 
		--sctemp = nvidiagpu.getGPUShaderClock()
		mctemp = 	nvidiagpu.getGPUMemoryClock()
	return string.format ( "%4i/%4i", cctemp, mctemp)
end

--printToLuaLCDConsole( "PerfData 0.210 DEBUG VERSION" )


while 1
do
printToGLCD( 0, 0, os.date( "%b %d,%Y %I:%M:%S%p (%a)"))

printToGLCD( 0, 2, "CPUt:" .. cputemp() )
printToGLCD( 0, 3, "CPUu:" .. perfcounter.getCPUUsage( ) )

printToGLCD( 0, 5, "freeMem:" .. perfcounter.getFreeRAM( ) )

printToGLCD( 0, 6, "GPUt:" .. gputemp() ..  " FANSPD: " .. nvidiagpu.getGPUGetCoolerSettings() .. "%")
printToGLCD( 0, 7, "GPUu:" .. gpuUseage() )

nvidiagpu.fillClockStruct(); -- must call this before caling any of the clock getting functions
printToGLCD( 0, 8, "GPUc " ..   gpuClocks())

printToGLCD( 0, 9, string.format ( "NETsend/recv:%8i/%8i", perfcounter.getNetSent( ), perfcounter.getNetRecv( ) ))
printToGLCD( 0, 10, string.format ( "PageFlt: %7i ", perfcounter.getPageFaults( ) ))

printToGLCD( 0, 11, string.format ( "Context: %7i ", perfcounter.getContext() ))
printToGLCD( 0, 12, string.format ( "Interut: %7i",  perfcounter.getInterrupts() ))

printToGLCD( 0, 13, string.format ( "DiskR: %9i ", perfcounter.getDiskReadBytes() ))
printToGLCD( 0, 14, string.format ( "DiskW: %9i",  perfcounter.getDiskWriteBytes() ))



--printToGLCD( 0, 7, string.format ( "NETr:%8i NETs: %8i", perfcounter.getNetRecv( ), perfcounter.getNetSent( ) )
--printToGLCD( 0, 15, "NETs:" .. perfcounter.getNetSent( ) )

end



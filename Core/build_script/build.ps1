param (
	$project,
	$system,
	$build,
	#$generator,
	$native_compiler_c,
	$native_compiler_cxx,
	$android_ndk,
	$android_arch,
	$android_platform
)

$generator = "Ninja"

function prepare {
	switch ($system) {
		"native" {
			if ($IsWindows -or ($IsWindows -eq $null)) {
				$global:system_name = "windows"
				$global:compiler_name = "msvc"
				$native_arch = (Get-Process -Id $PID).StartInfo.EnvironmentVariables["PROCESSOR_ARCHITECTURE"]
			}
			if ($IsLinux) {
				$global:system_name = "linux"
				$global:compiler_name = "clang"
				$native_arch = (& "arch")
			}
			if ($IsMacOs) {
				$global:system_name = "macos"
				$global:compiler_name = "clang"
				$native_arch = (& "uname" "-m")
			}
			switch ($native_arch) {
				"x86" {
					$global:arch_name = "x86_32"
				}
				"x86_64" {
					$global:arch_name = "x86_64"
				}
				"amd64" {
					$global:arch_name = "x86_64"
				}
				default {
					throw "unknown native_arch : $native_arch"
				}
			}
		}
		"android" {
			$global:system_name = "android"
			$global:compiler_name = "clang"
			switch ($android_arch) {
				"arm_32_v7a" {
					$global:arch_name = "arm_32_v7a"
					$global:android_arch_cmake = "armeabi-v7a"
				}
				"arm_64_v8a" {
					$global:arch_name = "arm_64_v8a"
					$global:android_arch_cmake = "arm64-v8a"
				}
				"x86_32" {
					$global:arch_name = "x86_32"
					$global:android_arch_cmake = "x86"
				}
				"x86_64" {
					$global:arch_name = "x86_64"
					$global:android_arch_cmake = "x86_64"
				}
				default {
					throw "-android_arch should be 'arm_32_v7a' or 'arm_64_v8a' or 'x86_32' or 'x86_64' : $android_arch"
				}
			}
		}
		default {
			throw "-system should be 'native' or 'android' : $system"
		}
	}
	switch ($build) {
		"debug" {
			$global:build_name = "debug"
			$global:build_name_cmake = "Debug"
		}
		"release" {
			$global:build_name = "release"
			$global:build_name_cmake = "Release"
		}
		"release_minimum" {
			$global:build_name = "release_minimum"
			$global:build_name_cmake = "MinSizeRel"
		}
		"release_debug" {
			$global:build_name = "release_debug"
			$global:build_name_cmake = "RelWithDebInfo"
		}
		default {
			throw "-build should be 'debug' or 'release' or 'release_minimum' or 'release_debug'"
		}
	}
	$global:project_source = "$project"
	$global:project_output = "$project/.build/$system_name.$arch_name.$compiler_name.$build_name"
}
function check_command_exist {
	param ($command)
	try {
		if (Get-Command "$command" -errorAction "SilentlyContinue") {
			return $true
		}
	} catch {
		return $false;
	}
}
function check_required_tool {
	if (!(check_command_exist "cmake")) {
		throw "cmake is not found"
	}
	if ($system -eq "native") {
		if (!(check_command_exist "ninja")) {
			throw "ninja is not found"
		}
		if (!(check_command_exist "$native_compiler_c")) {
			throw "native_compiler_c is not found : $native_compiler_c"
		}
		if (!(check_command_exist "$native_compiler_cxx")) {
			throw "native_compiler_cxx is not found : $native_compiler_cxx"
		}
	}
	if ($system -eq "android") {
		$global:android_ndk_toolchain_cmake = "$android_ndk/build/cmake/android.toolchain.cmake"
		if (!(Test-Path "$android_ndk")) {
			throw "android_ndk is not found : $android_ndk"
		}
		if (!(Test-Path "$android_ndk_toolchain_cmake")) {
			throw "android_ndk_toolchain_cmake is not found : $android_ndk_toolchain_cmake"
		}
	}
}
function cmake_clear {
	if (Test-Path "$project_output") {
		Remove-Item "$project_output" -Recurse
	}
}
function cmake_generate {
	switch ($system) {
		"native" {
			& "cmake" `
				"-S" "$project_source" `
				"-B" "$project_output" `
				"-D" "CMAKE_C_COMPILER=$native_compiler_c" `
				"-D" "CMAKE_CXX_COMPILER=$native_compiler_cxx" `
				"-D" "CMAKE_BUILD_TYPE=$build_name_cmake" `
				"-G" "$generator"
		}
		"android" {
			& "cmake" `
				"-S" "$project_source" `
				"-B" "$project_output" `
				"-D" "CMAKE_SYSTEM_NAME=Android" `
				"-D" "CMAKE_SYSTEM_VERSION=$android_platform" `
				"-D" "CMAKE_ANDROID_ARCH_ABI=$android_arch_cmake" `
				"-D" "CMAKE_ANDROID_NDK=$android_ndk" `
				"-D" "CMAKE_ANDROID_STL_TYPE=c++_shared" `
				"-D" "CMAKE_BUILD_TYPE=$build_name_cmake" `
				"-G" "$generator"
		}
	}
}
function cmake_build {
	& "cmake" `
		"--build" "$project_output"
}

Write-Host "step : prepare"
prepare
Write-Host "step finish"

Write-Host "target : $system_name.$arch_name.$compiler_name.$build_name"

Write-Host "step : check"
check_required_tool
Write-Host "step finish"

Write-Host "step : clear"
cmake_clear
Write-Host "step finish"

Write-Host "step : generate"
cmake_generate
Write-Host "step finish"

Write-Host "step : build"
cmake_build
Write-Host "step finish"

Write-Host "done : $project_output"

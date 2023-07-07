cmake_minimum_required(VERSION 3.5.1)
project(cdron_video_analysis)

set(CMAKE_CXX_STANDARD 14)
# It prevents the decay to C++98 when the compiler does not support C++14
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# It disables the use of compiler-specific extensions
# e.g. -std=c++14 rather than -std=gnu++14
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Try to find OpenCV
# set(OpenCV_DIR ....)
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    # If the package has been found, several variables will
    # be set, you can find the full list with descriptions
    # in the OpenCVConfig.cmake file.
    # Print some message showing some of them
    message(STATUS "OpenCV library status:")
    message(STATUS "    version: ${OpenCV_VERSION}")
    message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}" \n)
else ()
    message(FATAL_ERROR "Could not locate OpenCV" \n)
endif()

set(Torch_DIR libtorch/share/cmake/Torch)
find_package(Torch PATHS ${Torch_DIR} NO_DEFAULT REQUIRED)
if (Torch_FOUND)
    message(STATUS "Torch library found!")
    message(STATUS "    include path: ${TORCH_INCLUDE_DIRS}" \n)
else ()
    message(FATAL_ERROR "Could not locate Torch" \n)
endif()

#include_directories(${PROJECT_SOURCE_DIR}/include)
#
#
# INFERENCE_HELPER_ENABLE_MNN

#D:\Work\cartificial_intelligence\OpenCV\build_20230706_vs2019\lib\Release\opencv_world460.lib
#D:\Work\cartificial_intelligence\OpenCV\build_20230706_vs2019\lib\Release\opencv_img_hash460.lib
#D:\Work\cai\cdron_video_analysis\libtorch\lib\c10.lib
#D:\Work\cai\cdron_video_analysis\libtorch\lib\kineto.lib
#C:\Program Files\NVIDIA Corporation\NvToolsExt\\lib\x64\nvToolsExt64_1.lib
#C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\cudart_static.lib
#D:\Work\cai\cdron_video_analysis\libtorch\lib\caffe2_nvrtc.lib
#D:\Work\cai\cdron_video_analysis\libtorch\lib\c10_cuda.lib
#D:\Work\cai\cdron_video_analysis\libtorch\lib\torch.lib
#D:\Work\cai\cdron_video_analysis\libtorch\lib\torch_cuda.lib
#D:\Work\cai\cdron_video_analysis\libtorch\lib\torch_cpu.lib
#-INCLUDE:?warp_size@cuda@at@@YAHXZ
#C:\Program Files\NVIDIA Corporation\NvToolsExt\lib\x64\nvToolsExt64_1.lib
#C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\cufft.lib
#C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\curand.lib
#C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\cublas.lib
#C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\cublasLt.lib
 #D:\Work\cartificial_intelligence\OpenCV\build_20230706_vs2019;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudev\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\core\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudaarithm\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\flann\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\imgproc\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\intensity_transform\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\ml\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\phase_unwrapping\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\plot\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\quality\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\reg\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\surface_matching\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudafilters\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudaimgproc\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudawarping\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\dnn\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\dnn_superres\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\features2d\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\fuzzy\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\hfs\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\imgcodecs\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\line_descriptor\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\photo\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\saliency\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\text\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\videoio\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\wechat_qrcode\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\xphoto\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\barcode\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\calib3d\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudacodec\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudafeatures2d\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudastereo\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\datasets\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\highgui\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\mcc\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\objdetect\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\rapid\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\rgbd\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\shape\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\structured_light\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\video\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\xfeatures2d\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\ximgproc\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\xobjdetect\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\aruco\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\bgsegm\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\bioinspired\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\ccalib\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudabgsegm\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudalegacy\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudaobjdetect\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\dnn_objdetect\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\dpm\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\face\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\gapi\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\optflow\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\stitching\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\tracking\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudaoptflow\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\stereo\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\superres\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\videostab\include;
 #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\world\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\img_hash\include;
 #D:\Work\cai\cdron_video_analysis\libtorch\include;
 #D:\Work\cai\cdron_video_analysis\libtorch\include\torch\csrc\api\include;
 #C:\Program Files\NVIDIA Corporation\NvToolsExt\\include;
 #C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\include;
 #C:\Program Files\NVIDIA Corporation\NvToolsExt\include;
file(GLOB SOURCE_FILES 
	./*.h 
	./*.cpp
	)

file(GLOB buffer_module  
				HyperLPR/buffer_module/*.h
				HyperLPR/buffer_module/*.cpp )	
				
	


file(GLOB context_module  
				HyperLPR/context_module/*.h
				HyperLPR/context_module/*.cpp )	
				
				
file(GLOB inference_helper_module  
				HyperLPR/inference_helper_module/*.h
				HyperLPR/inference_helper_module/*.cpp )

file(GLOB nn_implementation_module  
				HyperLPR/nn_implementation_module/*.h
				HyperLPR/nn_implementation_module/*.cpp
				HyperLPR/nn_implementation_module/classification/*.h
				HyperLPR/nn_implementation_module/classification/*.cpp
				HyperLPR/nn_implementation_module/detect/*.h
				HyperLPR/nn_implementation_module/detect/*.cpp
				HyperLPR/nn_implementation_module/recognition/*.h
				HyperLPR/nn_implementation_module/recognition/*.cpp
				 )

file(GLOB nn_module  
				HyperLPR/nn_module/*.h
				HyperLPR/nn_module/*.cpp )	
file(GLOB c_api  
				HyperLPR/c_api/*.h
				HyperLPR/c_api/*.cpp )				
	
file(GLOB HyperLPR  
				HyperLPR/*.h 
			${buffer_module}
			${context_module}
			${inference_helper_module}
			${nn_implementation_module}
			${nn_module}
			${c_api}
			
			)

add_executable(${CMAKE_PROJECT_NAME} 
				${SOURCE_FILES}
				${HyperLPR})

target_link_libraries (
        ${CMAKE_PROJECT_NAME}
        ${OpenCV_LIBS}
        ${TORCH_LIBRARIES}
)

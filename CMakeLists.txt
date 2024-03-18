
project(test_torch)
MESSAGE(STATUS "#########################")
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_VERBOSE_MAKEFILE ON)
ADD_DEFINITIONS(-std=c++17 -g -Wall   )

ADD_DEFINITIONS(-D_GLIBCXX_USE_CXX11_ABI=0)

SET(EXTRA_INC_DIR /root/dep)



INCLUDE_DIRECTORIES(
        /usr/include
        /usr/local/include
        /root/Work/libtorch/include
        /root/Work/libtorch/include/torch/csrc/api/include
        /usr/local/include/opencv4
        /usr/local/cuda-11.8/include
		${EXTRA_INC_DIR}/boost/include 
		${EXTRA_INC_DIR}/libhv/include 
		${EXTRA_INC_DIR}/jsoncpp/include 
		./cdron_video_analysis/
		./cdron_video_analysis/HyperLPR/ 
		 
		 ./cdron_video_analysis/HyperLPR/buffer_module/ 
		 
		 ./cdron_video_analysis/HyperLPR/c_api/ 
		 
		 
		 
		 ./cdron_video_analysis/HyperLPR/context_module/ 
		 
		 
		 ./cdron_video_analysis/HyperLPR/inference_helper_module/ 
		 
		 
		 ./cdron_video_analysis/HyperLPR/nn_implementation_module/ 
		 
		 
		 ./cdron_video_analysis/HyperLPR/nn_implementation_module/classification/ 
		 
		 
		 ./cdron_video_analysis/HyperLPR/nn_implementation_module/detect/ 
		 
		 
		 ./cdron_video_analysis/HyperLPR/nn_implementation_module/recognition/ 
		 
		 
		 ./cdron_video_analysis/HyperLPR/nn_module/ 

        )


LINK_DIRECTORIES(
        /usr/local/lib
        /usr/local/lib64
        /usr/lib/x86_64-linux-gnu
        /usr/local/cuda-11.8/lib64
        /root/Work/libtorch/lib
        /usr/local/lib
        ${PROJECT_SOURCE_DIR}/linux/lib
		${EXTRA_INC_DIR}/boost/lib 
		${EXTRA_INC_DIR}/libhv/lib 
		${EXTRA_INC_DIR}/jsoncpp/lib 

)


file(GLOB ALL_CXX_SRCS
                 ./*.h
                 ./*.cpp
                 ./*.hpp
                 ./*.cc
				 
				 ./cdron_video_analysis/*.h
                 ./cdron_video_analysis/*.cpp
                 ./cdron_video_analysis/*.hpp
                 ./cdron_video_analysis/*.cc
				 
				 
				 ###
				 ./cdron_video_analysis/HyperLPR/*.h
                 ./cdron_video_analysis/HyperLPR/*.cpp
                 ./cdron_video_analysis/HyperLPR/*.hpp
                 ./cdron_video_analysis/HyperLPR/*.cc
				 
				 ./cdron_video_analysis/HyperLPR/buffer_module/*.h
                 ./cdron_video_analysis/HyperLPR/buffer_module/*.cpp
                 ./cdron_video_analysis/HyperLPR/buffer_module/*.hpp
                 ./cdron_video_analysis/HyperLPR/buffer_module/*.cc
				 
				 ./cdron_video_analysis/HyperLPR/c_api/*.h
                 ./cdron_video_analysis/HyperLPR/c_api/*.cpp
                 ./cdron_video_analysis/HyperLPR/c_api/*.hpp
                 ./cdron_video_analysis/HyperLPR/c_api/*.cc
				 
				 
				 
				 ./cdron_video_analysis/HyperLPR/context_module/*.h
                 ./cdron_video_analysis/HyperLPR/context_module/*.cpp
                 ./cdron_video_analysis/HyperLPR/context_module/*.hpp
                 ./cdron_video_analysis/HyperLPR/context_module/*.cc
				 
				 
				 ./cdron_video_analysis/HyperLPR/inference_helper_module/*.h
                 ./cdron_video_analysis/HyperLPR/inference_helper_module/*.cpp
                 ./cdron_video_analysis/HyperLPR/inference_helper_module/*.hpp
                 ./cdron_video_analysis/HyperLPR/inference_helper_module/*.cc
				 
				 
				 ./cdron_video_analysis/HyperLPR/nn_implementation_module/*.h
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/*.cpp
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/*.hpp
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/*.cc
				 
				 
				 ./cdron_video_analysis/HyperLPR/nn_implementation_module/classification/*.h
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/classification/*.cpp
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/classification/*.hpp
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/classification/*.cc
				 
				 
				 ./cdron_video_analysis/HyperLPR/nn_implementation_module/detect/*.h
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/detect/*.cpp
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/detect/*.hpp
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/detect/*.cc
				 
				 
				 ./cdron_video_analysis/HyperLPR/nn_implementation_module/recognition/*.h
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/recognition/*.cpp
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/recognition/*.hpp
                 ./cdron_video_analysis/HyperLPR/nn_implementation_module/recognition/*.cc
				 
				 
				 ./cdron_video_analysis/HyperLPR/nn_module/*.h
                 ./cdron_video_analysis/HyperLPR/nn_module/*.cpp
                 ./cdron_video_analysis/HyperLPR/nn_module/*.hpp
                 ./cdron_video_analysis/HyperLPR/nn_module/*.cc
                 )

add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})



set(ALL_LIB     pthread

		libboost_atomic.a                
		libboost_exception.a             
		libboost_log.a                   
		libboost_math_tr1f.a             
		libboost_serialization.a         
		libboost_system.a                
		libboost_wave.a
		libboost_chrono.a                
		libboost_fiber.a                 
		libboost_log_setup.a             
		libboost_math_tr1l.a             
		libboost_signals.a               
		libboost_test_exec_monitor.a     
		libboost_wserialization.a
		libboost_container.a             
		libboost_filesystem.a            
		libboost_math_c99.a              
		libboost_prg_exec_monitor.a      
		libboost_stacktrace_addr2line.a  
		libboost_thread.a
		libboost_context.a               
		libboost_graph.a                 
		libboost_math_c99f.a             
		libboost_program_options.a       
		libboost_stacktrace_backtrace.a  
		libboost_timer.a
		libboost_coroutine.a             
		libboost_iostreams.a             
		libboost_math_c99l.a             
		libboost_random.a                
		libboost_stacktrace_basic.a      
		libboost_type_erasure.a
		libboost_date_time.a             
		libboost_locale.a                
		libboost_math_tr1.a              
		libboost_regex.a                 
		libboost_stacktrace_noop.a       
		libboost_unit_test_framework.a

		 #libhv.so
		 libhv_static.a
		libjsoncpp.a
        pthread
        #/usr/lib/x86_64-linux-gnu/libpng16.so


        libOpenCL.so    
		libcublas.so    
		libcudart.so  
		libcufftw.so  
		libcufile_rdma.so  
		libcurand.so    
		libcusolverMg.so  
		libnppc.so    
		libnppicc.so   
		libnppif.so  
		libnppim.so   
		libnppisu.so  
		libnpps.so        
		libnvblas.so  
		libnvrtc-builtins.so
        libaccinj64.so  
		libcublasLt.so  
		libcufft.so   
		libcufile.so  
		libcuinj64.so      
		libcusolver.so  
		libcusparse.so    
		libnppial.so  
		libnppidei.so  
		libnppig.so  
		libnppist.so  
		libnppitc.so  
		libnvToolsExt.so  
		libnvjpeg.so  
		libnvrtc.so



        ########OpenCV ########


        libopencv_imgcodecs.so
        libopencv_aruco.so
        libopencv_cudacodec.so
        libopencv_cudawarping.so
        libopencv_features2d.so
        libopencv_phase_unwrapping.so
        libopencv_shape.so
        libopencv_video.so
        libopencv_bgsegm.so
        libopencv_cudafeatures2d.so
        libopencv_cudev.so
        libopencv_flann.so
        libopencv_imgproc.so
        libopencv_photo.so
        libopencv_stereo.so
        libopencv_videoio.so
        libopencv_bioinspired.so
        libopencv_cudafilters.so
        libopencv_datasets.so
        libopencv_freetype.so
        libopencv_intensity_transform.so
        libopencv_plot.so
        libopencv_stitching.so
        libopencv_videostab.so
        libopencv_calib3d.so
        libopencv_cudaimgproc.so
        pthread
        libopencv_dnn.so
        libopencv_fuzzy.so
        libopencv_line_descriptor.so
        libopencv_quality.so
        libopencv_structured_light.so
        libopencv_wechat_qrcode.so
        libopencv_ccalib.so
        libopencv_cudalegacy.so
        libopencv_dnn_objdetect.so
        libopencv_gapi.so
        libopencv_mcc.so
        libopencv_rapid.so
        libopencv_superres.so
        libopencv_xfeatures2d.so
        libopencv_core.so
        libopencv_cudaobjdetect.so
        libopencv_dnn_superres.so
        libopencv_hfs.so
        libopencv_ml.so
        libopencv_reg.so
        libopencv_surface_matching.so
        libopencv_ximgproc.so
        libopencv_cudaarithm.so
        libopencv_cudaoptflow.so
        libopencv_dpm.so
        libopencv_highgui.so
        libopencv_objdetect.so
        libopencv_rgbd.so
        libopencv_text.so
        libopencv_xobjdetect.so
        libopencv_cudabgsegm.so
        libopencv_cudastereo.so
        libopencv_face.so
        libopencv_img_hash.so
        libopencv_optflow.so
        libopencv_saliency.so
        libopencv_tracking.so
        libopencv_xphoto.so

		libbackend_with_compiler.so  
		libc10_cuda.so        
		libcaffe2_nvrtc.so  
		libjitbackend_test.so  
		libpytorch_jni.so  
		libtorch.so      
		libtorch_cuda.so         
		libtorch_global_deps.so  
		libtorchbind_test.so
		libc10.so                    
		libc10d_cuda_test.so  
		libfbjni.so         
		libnnapi_backend.so    
		libshm.so          
		libtorch_cpu.so  
		libtorch_cuda_linalg.so  
		libtorch_python.so

        )




target_link_libraries(${PROJECT_NAME} ${ALL_LIB})



#cmake_minimum_required(VERSION 3.5.1)
#project(cdron_video_analysis)












#set(CMAKE_CXX_STANDARD 14)
## It prevents the decay to C++98 when the compiler does not support C++14
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
## It disables the use of compiler-specific extensions
## e.g. -std=c++14 rather than -std=gnu++14
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
#
## Try to find OpenCV
## set(OpenCV_DIR ....)
#find_package(OpenCV REQUIRED)
#if (OpenCV_FOUND)
#    # If the package has been found, several variables will
#    # be set, you can find the full list with descriptions
#    # in the OpenCVConfig.cmake file.
#    # Print some message showing some of them
#    message(STATUS "OpenCV library status:")
#    message(STATUS "    version: ${OpenCV_VERSION}")
#    message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}" \n)
#else ()
#    message(FATAL_ERROR "Could not locate OpenCV" \n)
#endif()
#
#set(Torch_DIR libtorch/share/cmake/Torch)
#find_package(Torch PATHS ${Torch_DIR} NO_DEFAULT REQUIRED)
#if (Torch_FOUND)
#    message(STATUS "Torch library found!")
#    message(STATUS "    include path: ${TORCH_INCLUDE_DIRS}" \n)
#else ()
#    message(FATAL_ERROR "Could not locate Torch" \n)
#endif()
#
##include_directories(${PROJECT_SOURCE_DIR}/include)
##
##
## INFERENCE_HELPER_ENABLE_MNN
#
##D:\Work\cartificial_intelligence\OpenCV\build_20230706_vs2019\lib\Release\opencv_world460.lib
##D:\Work\cartificial_intelligence\OpenCV\build_20230706_vs2019\lib\Release\opencv_img_hash460.lib
##D:\Work\cai\cdron_video_analysis\libtorch\lib\c10.lib
##D:\Work\cai\cdron_video_analysis\libtorch\lib\kineto.lib
##C:\Program Files\NVIDIA Corporation\NvToolsExt\\lib\x64\nvToolsExt64_1.lib
##C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\cudart_static.lib
##D:\Work\cai\cdron_video_analysis\libtorch\lib\caffe2_nvrtc.lib
##D:\Work\cai\cdron_video_analysis\libtorch\lib\c10_cuda.lib
##D:\Work\cai\cdron_video_analysis\libtorch\lib\torch.lib
##D:\Work\cai\cdron_video_analysis\libtorch\lib\torch_cuda.lib
##D:\Work\cai\cdron_video_analysis\libtorch\lib\torch_cpu.lib
##-INCLUDE:?warp_size@cuda@at@@YAHXZ
##C:\Program Files\NVIDIA Corporation\NvToolsExt\lib\x64\nvToolsExt64_1.lib
##C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\cufft.lib
##C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\curand.lib
##C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\cublas.lib
##C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\lib\x64\cublasLt.lib
# #D:\Work\cartificial_intelligence\OpenCV\build_20230706_vs2019;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudev\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\core\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudaarithm\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\flann\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\imgproc\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\intensity_transform\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\ml\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\phase_unwrapping\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\plot\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\quality\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\reg\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\surface_matching\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudafilters\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudaimgproc\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudawarping\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\dnn\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\dnn_superres\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\features2d\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\fuzzy\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\hfs\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\imgcodecs\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\line_descriptor\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\photo\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\saliency\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\text\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\videoio\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\wechat_qrcode\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\xphoto\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\barcode\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\calib3d\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudacodec\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudafeatures2d\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudastereo\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\datasets\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\highgui\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\mcc\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\objdetect\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\rapid\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\rgbd\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\shape\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\structured_light\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\video\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\xfeatures2d\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\ximgproc\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\xobjdetect\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\aruco\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\bgsegm\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\bioinspired\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\ccalib\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudabgsegm\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudalegacy\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudaobjdetect\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\dnn_objdetect\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\dpm\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\face\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\gapi\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\optflow\include;D:\Work\cartificial_intelligence\OpenCV\opencv\modules\stitching\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\tracking\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\cudaoptflow\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\stereo\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\superres\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\videostab\include;
# #D:\Work\cartificial_intelligence\OpenCV\opencv\modules\world\include;D:\Work\cartificial_intelligence\OpenCV\opencv_contrib\modules\img_hash\include;
# #D:\Work\cai\cdron_video_analysis\libtorch\include;
# #D:\Work\cai\cdron_video_analysis\libtorch\include\torch\csrc\api\include;
# #C:\Program Files\NVIDIA Corporation\NvToolsExt\\include;
# #C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\include;
# #C:\Program Files\NVIDIA Corporation\NvToolsExt\include;
#file(GLOB SOURCE_FILES 
#	./*.h 
#	./*.cpp
#	)
#
#file(GLOB buffer_module  
#				HyperLPR/buffer_module/*.h
#				HyperLPR/buffer_module/*.cpp )	
#				
#	
#
#
#file(GLOB context_module  
#				HyperLPR/context_module/*.h
#				HyperLPR/context_module/*.cpp )	
#				
#				
#file(GLOB inference_helper_module  
#				HyperLPR/inference_helper_module/*.h
#				HyperLPR/inference_helper_module/*.cpp )
#
#file(GLOB nn_implementation_module  
#				HyperLPR/nn_implementation_module/*.h
#				HyperLPR/nn_implementation_module/*.cpp
#				HyperLPR/nn_implementation_module/classification/*.h
#				HyperLPR/nn_implementation_module/classification/*.cpp
#				HyperLPR/nn_implementation_module/detect/*.h
#				HyperLPR/nn_implementation_module/detect/*.cpp
#				HyperLPR/nn_implementation_module/recognition/*.h
#				HyperLPR/nn_implementation_module/recognition/*.cpp
#				 )
#
#file(GLOB nn_module  
#				HyperLPR/nn_module/*.h
#				HyperLPR/nn_module/*.cpp )	
#file(GLOB c_api  
#				HyperLPR/c_api/*.h
#				HyperLPR/c_api/*.cpp )				
#	
#file(GLOB HyperLPR  
#				HyperLPR/*.h 
#			${buffer_module}
#			${context_module}
#			${inference_helper_module}
#			${nn_implementation_module}
#			${nn_module}
#			${c_api}
#			
#			)
#
#add_executable(${CMAKE_PROJECT_NAME} 
#				${SOURCE_FILES}
#				${HyperLPR})
#
#target_link_libraries (
#        ${CMAKE_PROJECT_NAME}
#        ${OpenCV_LIBS}
#        ${TORCH_LIBRARIES}
#)

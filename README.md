# cdron_video_analysis
dron video analysis


```
sudo docker commit -a "chensong" -m "Ubuntu 18.04 "  e78185a5dd9b ubuntu_base:v1.0


# use nvidia 
sudo docker run --gpus all -itd    --name ubuntu_drone_base --runtime=nvidia ubuntu_base:v1.0
```


Driver:   Not Selected
Toolkit:  Installed in /usr/local/cuda-11.8/

Please make sure that
 -   PATH includes /usr/local/cuda-11.8/bin
 -   LD_LIBRARY_PATH includes /usr/local/cuda-11.8/lib64, or, add /usr/local/cuda-11.8/lib64 to /etc/ld.so.conf and run ldconfig as root

To uninstall the CUDA Toolkit, run cuda-uninstaller in /usr/local/cuda-11.8/bin
***WARNING: Incomplete installation! This installation did not install the CUDA Driver. A driver of version at least 520.00 is required for CUDA 11.8 functionality to work.
To install the driver using this installer, run the following command, replacing <CudaInstaller> with the name of this run file:
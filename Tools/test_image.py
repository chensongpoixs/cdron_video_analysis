
import random;
import string;
#from requests_toolbelt import MultipartEncoder;
from requests_toolbelt import MultipartEncoder
import requests

# ?face_plugins=landmarks,gender,age
url = "http://192.168.2.20:8078/api/v1/recognition/recognize?face_plugins=landmarks,gender,age";  # 上传图片的URL
file_path = "D:/Work/cai/db/人脸_陈虹旭_2.jpg"  # 图片文件的路径


api_key = '00000000-0000-0000-0000-000000000002';
# "0a16386c-2609-4e37-9883-a6ec18555d2a"
header = {"Content-Type": "multipart/form-data", "x-api-key": api_key};

files = {
    'file': ('人脸_刘静_31.jpg', open(file_path, 'rb'), 'image/jpeg')   # 设置文件名、文件内容和文件类型
    
}


cookie = "k0XBxWqacTscUOqx8WzKI407vFiCNbb5MIw_CaEb:P9Msf8J8YpOMOow62fhLocCDihM=:eyJzY29wZSI6Imp1bGl5ZSIsImR";

boundary = '----WebKitFormBoundary' + ''.join(random.sample(string.ascii_letters + string.digits, 16));


m = MultipartEncoder(fields = files, boundary = boundary);


handler_data = {"Content-Type": m.content_type, "Cookie": cookie, "x-api-key": api_key};
 
    

response = requests.post(url, headers = handler_data, data=m);

print(response.json());

if response.status_code == 200:
    print("上传成功");
else:
    print("上传失败");
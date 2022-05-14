import cv2

def Resize_img(path,savat):
    img = cv2.imread(path, cv2.IMREAD_UNCHANGED)

    print('Original Dimensions : ', img.shape)

    scale_percent = 400  # percent of original size
    width = int(img.shape[1] * scale_percent / 100)
    height = int(img.shape[0] * scale_percent / 100)

    dim = (width, height)

    # resize image
    resized = cv2.resize(img, dim, interpolation=cv2.INTER_LANCZOS4)

    print('Resized Dimensions : ', resized.shape)

    # cv2.imshow("Resized image", resized)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()

    cv2.imwrite(savat,resized)

    print('Finished : ', savat)

if __name__ == "__main__":

    Total = 47

    for i in range(Total):
        path = 'downscaled/'+str(i)+'.bmp'
        savat = 'upscaled/'+str(i)+'.bmp'
        Resize_img(path,savat)

    # with open('Report.txt','a') as f:
    #     f.writelines('begin\n')
    #     f.writelines('end\n')

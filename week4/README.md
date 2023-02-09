to start the function type ./multiprocessing (mode) (filename) into the command line

mode:
    There are two mode in multiprocessing program
        Q3 - process method
            this will use fork to run the cipher function and to store ciphertext into folder named Q3
        Q4 - thread method
            this will run thread in thread and store ciphertext into folder named Q4

filename:
    This is the file we want to encode
        simply type in the name of the file

Example:
    ./multiprocessing Q3 myfile.txt
        This will run program in process method encoding file myfile.txt
        The output will be stored into folder Q3 
        File will be stored into seperate file with each sentence

    ./multiprocessing Q4 filewithnotermination.txt
        This will run program in thread method encoding file filewithnotermination.txt
        The output will be stored into folder Q4
        File will be stored into seperate file with each sentence

You can try  
    ./multiprocessing Q3 myfile.txt
    ./multiprocessing Q4 myfile.txt
    ./multiprocessing Q3 emptyfile.txt
    ./multiprocessing Q4 emptyfile.txt
    ./multiprocessing Q3 filewithnotermination.txt
    ./multiprocessing Q4 filewithnotermination.txt

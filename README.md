# Danielib
Face detector

## Compiling
```
$ git clone https://github.com/davisking/dlib.git lib/dlib
$ mkdir build
$ cd build/
$ cmake ..
$ cmake --build .
```
## Using
```
JNIDlib dlib = new JNIDlib();
dlib.init("shape_predictor_68_face_landmarks.dat");
Face[] detect = dlib.detect(imageFile);
for (Face f : detect) {
    for (Point p : f.part) {
        System.out.println(p.x + " " + p.y);
    }
}
dlib.release();
```

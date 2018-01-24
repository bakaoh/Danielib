package com.bakaoh;

public class JNIDlib {

    static {
        System.loadLibrary("dlibjni");
    }

    private long ctx;
    public native void init(String model);
    public native void release();
    public native Face[] detect(String filename);

    public static class Face {

        public Point[] part;

        public Face(Point[] part) {
            this.part = part;
        }
    }

    public static class Point {

        public int x, y;

        public Point(int x, int y) {
            this.x = x;
            this.y = y;
        }
    }
}

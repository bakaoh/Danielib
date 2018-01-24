#include <jni.h>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>

#include <iostream>
#include "com_bakaoh_JNIDlib.h"

using namespace dlib;
using namespace std;

typedef struct DetectorContext {
    frontal_face_detector detector;
    shape_predictor sp;
} DetectorContext;

static void set_ctx(JNIEnv *env, jobject thiz, void *ctx) {
    jclass cls = env->GetObjectClass(thiz);
    jfieldID fid = env->GetFieldID(cls, "ctx", "J");
    env->SetLongField(thiz, fid, (jlong) (uintptr_t) ctx);
}

static void *get_ctx(JNIEnv *env, jobject thiz) {
    jclass cls = env->GetObjectClass(thiz);
    jfieldID fid = env->GetFieldID(cls, "ctx", "J");
    return (void *) (uintptr_t) env->GetLongField(thiz, fid);
}

#define DLIB_JNI_METHOD(METHOD_NAME) Java_com_bakaoh_JNIDlib_##METHOD_NAME

JNIEXPORT void JNICALL DLIB_JNI_METHOD(init)(JNIEnv * env, jobject thiz, jstring modelFile) {
    DetectorContext *ctx = (DetectorContext *) calloc(1, sizeof (DetectorContext));
    set_ctx(env, thiz, ctx);

    ctx->detector = get_frontal_face_detector();
    const char *file = env->GetStringUTFChars(modelFile, 0);
    deserialize(file) >> ctx->sp;
    env->ReleaseStringUTFChars(modelFile, file);
}

JNIEXPORT void JNICALL DLIB_JNI_METHOD(release)(JNIEnv * env, jobject thiz) {
    DetectorContext *ctx = (DetectorContext *) get_ctx(env, thiz);
    set_ctx(env, thiz, 0);
    free(ctx);
}

JNIEXPORT jobjectArray JNICALL DLIB_JNI_METHOD(detect)(JNIEnv * env, jobject thiz, jstring imageFile) {
    DetectorContext *ctx = (DetectorContext *) get_ctx(env, thiz);
    const char *file = env->GetStringUTFChars(imageFile, 0);

    try {

        array2d<rgb_pixel> img;
        load_image(img, file);

        // Now tell the face detector to give us a list of bounding boxes
        // around all the faces in the image.
        std::vector<rectangle> dets = ((frontal_face_detector) ctx->detector)(img);
        
        jclass faceCls = env->FindClass("com/bakaoh/JNIDlib$Face");
        jmethodID faceCons = env->GetMethodID(faceCls, "<init>", "([Lcom/bakaoh/JNIDlib$Point;)V");
        jclass pointCls = env->FindClass("com/bakaoh/JNIDlib$Point");
        jmethodID pointCons = env->GetMethodID(pointCls, "<init>", "(II)V");

        jobjectArray ret = (jobjectArray) env->NewObjectArray(dets.size(), faceCls, NULL);

        // Now we will go ask the shape_predictor to tell us the pose of
        // each face we detected.
        for (unsigned long j = 0; j < dets.size(); ++j) {
            full_object_detection shape = ctx->sp(img, dets[j]);
            jobjectArray points = (jobjectArray) env->NewObjectArray(shape.num_parts(), pointCls, NULL);
            for (unsigned long k = 0; k < shape.num_parts(); k++) {
                env->SetObjectArrayElement(points, k, env->NewObject(pointCls, pointCons, shape.part(k).x(), shape.part(k).y()));
            }
            env->SetObjectArrayElement(ret, j, env->NewObject(faceCls, faceCons, points));
        }

        env->ReleaseStringUTFChars(imageFile, file);
        return ret;
    } catch (exception& e) {
        env->ReleaseStringUTFChars(imageFile, file);
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), e.what());
    }
}


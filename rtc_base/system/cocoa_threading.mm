/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "rtc_base/system/cocoa_threading.h"

#import <Foundation/Foundation.h>

#include "rtc_base/checks.h"

void InitCocoaMultiThreading() {
  //使用NSThread的isMultiThreaded 来检查程序是否已经启动了Cocoa多线程，并把结果存储在 静态变量is_cocoa_multithreaded中
  static BOOL is_cocoa_multithreaded = [NSThread isMultiThreaded];
  if (!is_cocoa_multithreaded) { //如果没有启动 Cocoa 多线程
    // +[NSObject class] is idempotent.
    [NSThread detachNewThreadSelector:@selector(class) toTarget:[NSObject class] withObject:nil]; //创建一个新的线程，调用NSObject的class方法，这个调用目的是启动Cocoa多线程
    is_cocoa_multithreaded = YES; //重置这个标志位
    RTC_DCHECK([NSThread isMultiThreaded]); //确保多线程已经启动
  }
}

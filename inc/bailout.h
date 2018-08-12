/** 函数内异常处理  -- 少用
 *
 * example:
 *     fd1 = open(filename1, O_RDONLY);
 *     if( fd1 < 0 )  BAILOUT(open_filename1);
 *
 *     fd2 = open(filename2, O_RDONLY); 
 *     if( fd2 < 0 )  BAILOUT(open_filename2);
 *     
 *     fd3 = open(filename3, O_RDONLY); 
 *     if( fd3 < 0 )  BAILOUT(open_filename3);
 *
 *     ...
 *     return 0;
 *
 *     ON_ERROR(open_filename3):
 *          free(fd1);
 *          free(fd2);
 *     ON_ERROR(open_filename2):
 *          free(fd1);
 *     ON_ERROR(open_filename1):
 *          return -1;
 * } // end of function
 */




#ifndef __sc_bailout_h
#define __sc_bailout_h

#define BAILOUT(_label)   { goto _bailout_##_label; }

#define ON_ERROR(_label)  _bailout_##_label

#endif

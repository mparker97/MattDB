#ifndef PARSE_CLASS_H
#define PARSE_CLASS_H

typedef unsigned short parse_class_t;

#define _PC_REGEX_OPEN 256
#define _PC_REGEX_CLOSE 257
#define _PC_SPACE 258

#define _PCS_SPACE (parse_class_t){_PC_REGEX_OPEN, '[', ' ', '\t', \n', ']', '+', _PC_REGEX_CLOSE, 0}

#endif

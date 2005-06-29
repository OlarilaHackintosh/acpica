/******************************************************************************
 *
 * Module Name: cmclib - Local implementation of C library functions
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999, Intel Corp.  All rights
 * reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights.  You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code.  No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision.  In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change.  Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee.  Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution.  In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE.  ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT,  ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES.  INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS.  INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government.  In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/


#define __CMCLIB_C__

#include "acpi.h"
#include "events.h"
#include "hardware.h"
#include "namesp.h"
#include "interp.h"
#include "amlcode.h"

/*
 * These implementations of standard C Library routines can optionally be
 * used if a C library is not available.  In general, they are less efficient
 * than an inline or assembly implementation
 */

#define _COMPONENT          MISCELLANEOUS
        MODULE_NAME         ("cmclib");


#ifdef _MSC_VER                 /* disable some level-4 warnings for VC++ */
#pragma warning(disable:4706)   /* warning C4706: assignment within conditional expression */
#endif


/*******************************************************************************
 *
 * FUNCTION:    strlen
 *
 * PARAMETERS:  String              - Null terminated string
 *
 * RETURN:      Length
 *
 * DESCRIPTION: Returns the length of the input string
 *
 ******************************************************************************/


ACPI_SIZE
AcpiCmStrlen (
    const char              *String)
{
    UINT32                  Length = 0;


    /* Count the string until a null is encountered */

    while (*String)
    {
        Length++;
        String++;
    }

    return Length;
}


/*******************************************************************************
 *
 * FUNCTION:    strcpy
 *
 * PARAMETERS:  DstString       - Target of the copy
 *              SrcString       - The source string to copy
 *
 * RETURN:      DstString
 *
 * DESCRIPTION: Copy a null terminated string
 *
 ******************************************************************************/

char *
AcpiCmStrcpy (
    char                    *DstString,
    const char              *SrcString)
{
    char                    *String = DstString;


    /* Move bytes brute force */

    while (*SrcString)
    {
        *String = *SrcString;

        String++;
        SrcString++;
    }

    /* Null terminate */

    *String = 0;

    return DstString;
}


/*******************************************************************************
 *
 * FUNCTION:    strncpy
 *
 * PARAMETERS:  DstString       - Target of the copy
 *              SrcString       - The source string to copy
 *              Count           - Maximum # of bytes to copy
 *
 * RETURN:      DstString
 *
 * DESCRIPTION: Copy a null terminated string, with a maximum length
 *
 ******************************************************************************/

char *
AcpiCmStrncpy (
    char                    *DstString,
    const char              *SrcString,
    ACPI_SIZE               Count)
{
    char                    *String = DstString;


    /* Copy the string */

    for (String = DstString; Count && (Count--, (*String++ = *SrcString++)); )
    {;}

    /* Pad with nulls if necessary */

    while (Count--)
    {
        *String = 0;
        String++;
    }

    /* Return original pointer */

    return DstString;
}


/*******************************************************************************
 *
 * FUNCTION:    strcmp
 *
 * PARAMETERS:  String1         - First string
 *              String2         - Second string
 *
 * RETURN:      Index where strings mismatched, or 0 if strings matched
 *
 * DESCRIPTION: Compare two null terminated strings
 *
 ******************************************************************************/

UINT32
AcpiCmStrcmp (
    const char              *String1,
    const char              *String2)
{


    for ( ; (*String1 == *String2); String2++)
    {
        if (!*String1++)
        {
            return 0;
        }
    }


    return (unsigned char) *String1 - (unsigned char) *String2;
}


/*******************************************************************************
 *
 * FUNCTION:    strncmp
 *
 * PARAMETERS:  String1         - First string
 *              String2         - Second string
 *              Count           - Maximum # of bytes to compare
 *
 * RETURN:      Index where strings mismatched, or 0 if strings matched
 *
 * DESCRIPTION: Compare two null terminated strings, with a maximum length
 *
 ******************************************************************************/

UINT32
AcpiCmStrncmp (
    const char              *String1,
    const char              *String2,
    ACPI_SIZE               Count)
{


    for ( ; Count-- && (*String1 == *String2); String2++)
    {
        if (!*String1++)
        {
            return 0;
        }
    }

    return (Count == -1) ? 0 : ((unsigned char) *String1 -
        (unsigned char) *String2);
}


/*******************************************************************************
 *
 * FUNCTION:    Strcat
 *
 * PARAMETERS:  DstString       - Target of the copy
 *              SrcString       - The source string to copy
 *
 * RETURN:      DstString
 *
 * DESCRIPTION: Append a null terminated string to a null terminated string
 *
 ******************************************************************************/

char *
AcpiCmStrcat (
    char                    *DstString,
    const char              *SrcString)
{
    char                    *String;


    /* Find end of the destination string */

    for (String = DstString; *String++; )
    { ; }

    /* Concatinate the string */

    for (--String; (*String++ = *SrcString++); )
    { ; }

    return DstString;
}


/*******************************************************************************
 *
 * FUNCTION:    strncat
 *
 * PARAMETERS:  DstString       - Target of the copy
 *              SrcString       - The source string to copy
 *              Count           - Maximum # of bytes to copy
 *
 * RETURN:      DstString
 *
 * DESCRIPTION: Append a null terminated string to a null terminated string,
 *              with a maximum count.
 *
 ******************************************************************************/

char *
AcpiCmStrncat (
    char                    *DstString,
    const char              *SrcString,
    ACPI_SIZE               Count)
{
    char                    *String;


    if (Count)
    {
        /* Find end of the destination string */

        for (String = DstString; *String++; )
        { ; }

        /* Concatinate the string */

        for (--String; (*String++ = *SrcString++) && --Count; )
        { ; }

        /* Null terminate if necessary */

        if (!Count)
        {
            *String = 0;
        }
    }

    return DstString;
}


/*******************************************************************************
 *
 * FUNCTION:    memcpy
 *
 * PARAMETERS:  Dest        - Target of the copy
 *              Src         - Source buffer to copy
 *              Count       - Number of bytes to copy
 *
 * RETURN:      Dest
 *
 * DESCRIPTION: Copy arbitrary bytes of memory
 *
 ******************************************************************************/

void *
AcpiCmMemcpy (
    void                    *Dest,
    const void              *Src,
    ACPI_SIZE               Count)
{
    char                    *New = (char *) Dest;
    char                    *Old = (char *) Src;


    while (Count)
    {
        *New = *Old;
        New++;
        Old++;
        Count--;
    }

    return Dest;
}


/*******************************************************************************
 *
 * FUNCTION:    memset
 *
 * PARAMETERS:  Dest        - Buffer to set
 *              Value       - Value to set each byte of memory
 *              Count       - Number of bytes to set
 *
 * RETURN:      Dest
 *
 * DESCRIPTION: Initialize a buffer to a known value.
 *
 ******************************************************************************/

void *
AcpiCmMemset (
    void                    *Dest,
    INT32                   Value,
    ACPI_SIZE               Count)
{
    char                    *New = (char *) Dest;


    while (Count)
    {
        *New = (char) Value;
        New++;
        Count--;
    }

    return Dest;
}


#define NEGATIVE    1
#define POSITIVE    0


#define _XA     0x00    /* extra alphabetic - not supported */
#define _XS     0x40    /* extra space */
#define _BB     0x00    /* BEL, BS, etc. - not supported */
#define _CN     0x20    /* CR, FF, HT, NL, VT */
#define _DI     0x04    /* '0'-'9' */
#define _LO     0x02    /* 'a'-'z' */
#define _PU     0x10    /* punctuation */
#define _SP     0x08    /* space */
#define _UP     0x01    /* 'A'-'Z' */
#define _XD     0x80    /* '0'-'9', 'A'-'F', 'a'-'f' */

const unsigned char _ctype[257] = {
    _CN,            /* 0x0      0.     */
    _CN,            /* 0x1      1.     */
    _CN,            /* 0x2      2.     */
    _CN,            /* 0x3      3.     */
    _CN,            /* 0x4      4.     */
    _CN,            /* 0x5      5.     */
    _CN,            /* 0x6      6.     */
    _CN,            /* 0x7      7.     */
    _CN,            /* 0x8      8.     */
    _CN|_SP,        /* 0x9      9.     */
    _CN|_SP,        /* 0xA     10.     */
    _CN|_SP,        /* 0xB     11.     */
    _CN|_SP,        /* 0xC     12.     */
    _CN|_SP,        /* 0xD     13.     */
    _CN,            /* 0xE     14.     */
    _CN,            /* 0xF     15.     */
    _CN,            /* 0x10    16.     */
    _CN,            /* 0x11    17.     */
    _CN,            /* 0x12    18.     */
    _CN,            /* 0x13    19.     */
    _CN,            /* 0x14    20.     */
    _CN,            /* 0x15    21.     */
    _CN,            /* 0x16    22.     */
    _CN,            /* 0x17    23.     */
    _CN,            /* 0x18    24.     */
    _CN,            /* 0x19    25.     */
    _CN,            /* 0x1A    26.     */
    _CN,            /* 0x1B    27.     */
    _CN,            /* 0x1C    28.     */
    _CN,            /* 0x1D    29.     */
    _CN,            /* 0x1E    30.     */
    _CN,            /* 0x1F    31.     */
    _XS|_SP,        /* 0x20    32. ' ' */
    _PU,            /* 0x21    33. '!' */
    _PU,            /* 0x22    34. '"' */
    _PU,            /* 0x23    35. '#' */
    _PU,            /* 0x24    36. '$' */
    _PU,            /* 0x25    37. '%' */
    _PU,            /* 0x26    38. '&' */
    _PU,            /* 0x27    39. ''' */
    _PU,            /* 0x28    40. '(' */
    _PU,            /* 0x29    41. ')' */
    _PU,            /* 0x2A    42. '*' */
    _PU,            /* 0x2B    43. '+' */
    _PU,            /* 0x2C    44. ',' */
    _PU,            /* 0x2D    45. '-' */
    _PU,            /* 0x2E    46. '.' */
    _PU,            /* 0x2F    47. '/' */
    _XD|_DI,        /* 0x30    48. '0' */
    _XD|_DI,        /* 0x31    49. '1' */
    _XD|_DI,        /* 0x32    50. '2' */
    _XD|_DI,        /* 0x33    51. '3' */
    _XD|_DI,        /* 0x34    52. '4' */
    _XD|_DI,        /* 0x35    53. '5' */
    _XD|_DI,        /* 0x36    54. '6' */
    _XD|_DI,        /* 0x37    55. '7' */
    _XD|_DI,        /* 0x38    56. '8' */
    _XD|_DI,        /* 0x39    57. '9' */
    _PU,            /* 0x3A    58. ':' */
    _PU,            /* 0x3B    59. ';' */
    _PU,            /* 0x3C    60. '<' */
    _PU,            /* 0x3D    61. '=' */
    _PU,            /* 0x3E    62. '>' */
    _PU,            /* 0x3F    63. '?' */
    _PU,            /* 0x40    64. '@' */
    _XD|_UP,        /* 0x41    65. 'A' */
    _XD|_UP,        /* 0x42    66. 'B' */
    _XD|_UP,        /* 0x43    67. 'C' */
    _XD|_UP,        /* 0x44    68. 'D' */
    _XD|_UP,        /* 0x45    69. 'E' */
    _XD|_UP,        /* 0x46    70. 'F' */
    _UP,            /* 0x47    71. 'G' */
    _UP,            /* 0x48    72. 'H' */
    _UP,            /* 0x49    73. 'I' */
    _UP,            /* 0x4A    74. 'J' */
    _UP,            /* 0x4B    75. 'K' */
    _UP,            /* 0x4C    76. 'L' */
    _UP,            /* 0x4D    77. 'M' */
    _UP,            /* 0x4E    78. 'N' */
    _UP,            /* 0x4F    79. 'O' */
    _UP,            /* 0x50    80. 'P' */
    _UP,            /* 0x51    81. 'Q' */
    _UP,            /* 0x52    82. 'R' */
    _UP,            /* 0x53    83. 'S' */
    _UP,            /* 0x54    84. 'T' */
    _UP,            /* 0x55    85. 'U' */
    _UP,            /* 0x56    86. 'V' */
    _UP,            /* 0x57    87. 'W' */
    _UP,            /* 0x58    88. 'X' */
    _UP,            /* 0x59    89. 'Y' */
    _UP,            /* 0x5A    90. 'Z' */
    _PU,            /* 0x5B    91. '[' */
    _PU,            /* 0x5C    92. '\' */
    _PU,            /* 0x5D    93. ']' */
    _PU,            /* 0x5E    94. '^' */
    _PU,            /* 0x5F    95. '_' */
    _PU,            /* 0x60    96. '`' */
    _XD|_LO,        /* 0x61    97. 'a' */
    _XD|_LO,        /* 0x62    98. 'b' */
    _XD|_LO,        /* 0x63    99. 'c' */
    _XD|_LO,        /* 0x64   100. 'd' */
    _XD|_LO,        /* 0x65   101. 'e' */
    _XD|_LO,        /* 0x66   102. 'f' */
    _LO,            /* 0x67   103. 'g' */
    _LO,            /* 0x68   104. 'h' */
    _LO,            /* 0x69   105. 'i' */
    _LO,            /* 0x6A   106. 'j' */
    _LO,            /* 0x6B   107. 'k' */
    _LO,            /* 0x6C   108. 'l' */
    _LO,            /* 0x6D   109. 'm' */
    _LO,            /* 0x6E   110. 'n' */
    _LO,            /* 0x6F   111. 'o' */
    _LO,            /* 0x70   112. 'p' */
    _LO,            /* 0x71   113. 'q' */
    _LO,            /* 0x72   114. 'r' */
    _LO,            /* 0x73   115. 's' */
    _LO,            /* 0x74   116. 't' */
    _LO,            /* 0x75   117. 'u' */
    _LO,            /* 0x76   118. 'v' */
    _LO,            /* 0x77   119. 'w' */
    _LO,            /* 0x78   120. 'x' */
    _LO,            /* 0x79   121. 'y' */
    _LO,            /* 0x7A   122. 'z' */
    _PU,            /* 0x7B   123. '{' */
    _PU,            /* 0x7C   124. '|' */
    _PU,            /* 0x7D   125. '}' */
    _PU,            /* 0x7E   126. '~' */
    _CN,            /* 0x7F   127.     */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0x80 to 0x8F    */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0x90 to 0x9F    */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xA0 to 0xAF    */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xB0 to 0xBF    */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xC0 to 0xCF    */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xD0 to 0xDF    */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xE0 to 0xEF    */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 /* 0xF0 to 0x100   */
};

#define IS_UPPER(c)  (_ctype[(unsigned char)(c)] & (_UP))
#define IS_LOWER(c)  (_ctype[(unsigned char)(c)] & (_LO))
#define IS_DIGIT(c)  (_ctype[(unsigned char)(c)] & (_DI))
#define IS_SPACE(c)  (_ctype[(unsigned char)(c)] & (_SP))


/*******************************************************************************
 *
 * FUNCTION:    AcpiCmToUpper
 *
 * PARAMETERS:
 *
 * RETURN:
 *
 * DESCRIPTION: Convert character to uppercase
 *
 ******************************************************************************/

INT32
AcpiCmToUpper (
    INT32                   c)
{

    return (IS_LOWER(c) ? ((c)-0x20) : (c));
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiCmToLower
 *
 * PARAMETERS:
 *
 * RETURN:
 *
 * DESCRIPTION: Convert character to lowercase
 *
 ******************************************************************************/

INT32
AcpiCmToLower (
    INT32                   c)
{

    return (IS_UPPER(c) ? ((c)+0x20) : (c));
}


/*******************************************************************************
 *
 * FUNCTION:    strupr
 *
 * PARAMETERS:  SrcString       - The source string to convert to
 *
 * RETURN:      SrcString
 *
 * DESCRIPTION: Convert string to uppercase
 *
 ******************************************************************************/

char *
AcpiCmStrupr (
    char                    *SrcString)
{
    char                    *String;


    /* Walk entire string, uppercasing the letters */

    for (String = SrcString; *String; )
    {
        *String = (char) AcpiCmToUpper (*String);
        String++;
    }


    return SrcString;
}


/*******************************************************************************
 *
 * FUNCTION:    strstr
 *
 * PARAMETERS:  String1       -
 *              String2
 *
 * RETURN:
 *
 * DESCRIPTION: Checks if String2 occurs in String1. This is not really a
 *              full implementation of strstr, only sufficient for command
 *              matching
 *
 ******************************************************************************/

char *
AcpiCmStrstr (
    char                    *String1,
    char                    *String2)
{
    char                    *String;


    if (AcpiCmStrlen (String2) > AcpiCmStrlen (String1))
    {
        return NULL;
    }

    /* Walk entire string, uppercasing the letters */

    for (String = String1; *String2; )
    {
        if (*String2 != *String)
        {
            return NULL;
        }

        String2++;
        String++;
    }


    return String1;
}


/*******************************************************************************
 *
 * FUNCTION:    strtoul
 *
 * PARAMETERS:  String          - Null terminated string
 *              Terminater      - Where a pointer to the terminating byte is returned
 *              Base            - Radix of the string
 *
 * RETURN:      Converted value
 *
 * DESCRIPTION: Convert a string into an unsigned value.
 *
 ******************************************************************************/

UINT32
AcpiCmStrtoul (
    const char              *String,
    char                    **Terminator,
    INT32                   Base)
{
    UINT32                  converted = 0;
    INT32                   index;
    UINT32                  sign;
    const char              *StringStart;
    UINT32                  ReturnValue = 0;
    ACPI_STATUS             Status = AE_OK;


    /*
     * Save the value of the pointer to the buffer's first
     * character, save the current errno value, and then
     * skip over any white space in the buffer:
     */
    StringStart = String;
    while (IS_SPACE (*String) || *String == '\t')
    {
        ++String;
    }

    /*
     * The buffer may contain an optional plus or minus sign.
     * If it does, then skip over it but remember what is was:
     */
    if (*String == '-')
    {
        sign = NEGATIVE;
        ++String;
    }

    else if (*String == '+')
    {
        ++String;
        sign = POSITIVE;
    }

    else
    {
        sign = POSITIVE;
    }

    /*
     * If the input parameter Base is zero, then we need to
     * determine if it is octal, decimal, or hexadecimal:
     */
    if (Base == 0)
    {
        if (*String == '0')
        {
            if (AcpiCmToLower (*(++String)) == 'x')
            {
                Base = 16;
                ++String;
            }

            else
            {
                Base = 8;
            }
        }

        else
        {
            Base = 10;
        }
    }

    else if (Base < 2 || Base > 36)
    {
        /*
         * The specified Base parameter is not in the domain of
         * this function:
         */
        goto done;
    }

    /*
     * For octal and hexadecimal bases, skip over the leading
     * 0 or 0x, if they are present.
     */
    if (Base == 8 && *String == '0')
    {
        String++;
    }

    if (Base == 16 && *String == '0' && AcpiCmToLower (*(++String)) == 'x')
    {
        String++;
    }


    /*
     * Main loop: convert the string to an unsigned long:
     */
    while (*String)
    {
        if (IS_DIGIT (*String))
        {
            index = *String - '0';
        }

        else
        {
            index = AcpiCmToUpper (*String);
            if (IS_UPPER (index))
            {
                index = index - 'A' + 10;
            }

            else
            {
                goto done;
            }
        }

        if (index >= Base)
        {
            goto done;
        }

        /*
         * Check to see if value is out of range:
         */

        if (ReturnValue > ((ACPI_UINT32_MAX - (UINT32) index) /
                            (UINT32) Base))
        {
            Status = AE_ERROR;
            ReturnValue = 0L;           /* reset */
        }

        else
        {
            ReturnValue *= Base;
            ReturnValue += index;
            converted = 1;
        }

        ++String;
    }

done:
    /*
     * If appropriate, update the caller's pointer to the next
     * unconverted character in the buffer.
     */
    if (Terminator)
    {
        if (converted == 0 && ReturnValue == 0L && String != NULL)
        {
            *Terminator = (char *) StringStart;
        }

        else
        {
            *Terminator = (char *) String;
        }
    }

    if (Status == AE_ERROR)
    {
        ReturnValue = ACPI_UINT32_MAX;
    }

    /*
     * If a minus sign was present, then "the conversion is negated":
     */
    if (sign == NEGATIVE)
    {
        ReturnValue = (ACPI_UINT32_MAX - ReturnValue) + 1;
    }

    return (ReturnValue);
}


/******************************************************************************
 * 
 * Module Name: isnames - interpreter/scanner name load/execute
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

#define __ISNAMES_C__

#include <acpi.h>
#include <interpreter.h>
#include <amlcode.h>
#include <namespace.h>

#define _THIS_MODULE        "isnames.c"
#define _COMPONENT          INTERPRETER


#define PKG_Type1   64              /*  or 0x40 Max encoding size = 0x3F    */
#define PKG_Type2   16384           /*  or 0x4000 Max encoding size = 0xFFF */
#define PKG_Type3   4194304         /*  or 0x400000 Max encoding size = 0xFFFFF */
#define PKG_Type4   1073741824      /*  or 0x40000000 Max encoding size = 0xFFFFFFF */


/*****************************************************************************
 *
 * FUNCTION:    AmlAllocateNameString
 *
 * PARAMETERS:  PrefixCount         - Count of parent levels. Special cases:
 *                                    (-1) = root,  0 = none
 *              NumNameSegs         - count of 4-character name segments
 *
 * RETURN:      A pointer to the allocated string segment.  This segment must
 *              be deleted by the caller.
 *
 * DESCRIPTION: Ensure allocated name string is long enough,
 *              and set up prefix if any.
 *
 ****************************************************************************/

char *
AmlAllocateNameString (
    INT32                   PrefixCount, 
    INT32                   NumNameSegs)
{
    char                    *TempPtr;
    char                    *NameString;
    INT32                   SizeNeeded;


    FUNCTION_TRACE ("AmlAllocateNameString");

    /* 
     * Allow room for all \ and ^ prefixes, all segments, and a MultiNamePrefix.
     * This may actually be somewhat longer than needed.
     */

    if (PrefixCount < 0)
    {
        SizeNeeded = 1 /* root */ +  4 * NumNameSegs + 2;
    }
    else
    {
        SizeNeeded = PrefixCount + 4 * NumNameSegs + 2;
    }

    if (SizeNeeded < INITIAL_NAME_BUF_SIZE)
    {
        SizeNeeded = INITIAL_NAME_BUF_SIZE;
    }


    /* 
     * Allocate a buffer for the name.
     * This buffer must be deleted by the caller!
     */

    NameString = CmAllocate ((ACPI_SIZE) SizeNeeded);
    if (!NameString)
    {
        /* Allocation failure  */

        REPORT_ERROR ("AmlAllocateNameString: name allocation failure");
        return_VALUE (NULL);
    }

    TempPtr = NameString;

    /* Set up Root or Parent prefixes if needed */

    if (PrefixCount < 0)
    {
        *TempPtr++ = AML_RootPrefix;
    }

    else
    {
        while (PrefixCount--)
        {
            *TempPtr++ = AML_ParentPrefix;
        }
    }


    /* Set up Dual or Multi prefixes if needed */
    
    if (NumNameSegs > 2)
    {
        /* Set up multi prefixes   */

        *TempPtr++ = AML_MultiNamePrefixOp;
        *TempPtr++ = (char) NumNameSegs;
    }

    else if (2 == NumNameSegs)
    {
        /* Set up dual prefixes */

        *TempPtr++ = AML_DualNamePrefix;
    }

    /* Terminate string following prefixes. AmlDoSeg() will append the segment(s) */

    *TempPtr = 0;

    return_VALUE (NameString);
}


/*****************************************************************************
 *
 * FUNCTION:    AmlGoodChar
 *
 * PARAMETERS:  Character           - The character to be examined
 *
 * RETURN:      1 if Character may appear in a name, else 0
 *
 * DESCRIPTION: Check for a printable character
 *
 ****************************************************************************/

INT32 
AmlGoodChar (
    INT32                   Character)
{

    return ((Character == '_') || (Character >= 'A' && Character <= 'Z') ||
                (Character >= '0' && Character <= '9'));
}


/*****************************************************************************
 * 
 * FUNCTION:    AmlDecodePackageLength
 *
 * PARAMETERS:  LastPkgLen          - latest value decoded by DoPkgLength() for
 *                                    most recently examined package or field
 *
 * RETURN:      Number of bytes contained in package length encoding
 *
 * DESCRIPTION: Decodes the Package Length. Upper 2 bits are are used to
 *              tell if type 1, 2, 3, or 4.
 *                  0x3F        = Max 1 byte encoding,
 *                  0xFFF       = Max 2 byte encoding,
 *                  0xFFFFF     = Max 3 Byte encoding,
 *                  0xFFFFFFFFF = Max 4 Byte encoding.
 *
 ****************************************************************************/

INT32 
AmlDecodePackageLength (
    INT32                   LastPkgLen)
{
    INT32                   NumBytes = 0;


    FUNCTION_TRACE ("AmlDecodePackageLength");


    if (LastPkgLen < PKG_Type1)
    {
        NumBytes = 1;
    }

    else if (LastPkgLen < PKG_Type2)
    {
        NumBytes = 2;
    }

    else if (LastPkgLen < PKG_Type3)
    {
        NumBytes = 3;
    }

    else if (LastPkgLen < PKG_Type4)
    {
        NumBytes = 4;
    }

    return_VALUE (NumBytes);
}


/*****************************************************************************
 *
 * FUNCTION:    AmlDoSeg
 *
 * PARAMETERS:  LoadExecMode        - MODE_Load or MODE_Exec
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Print/exec a name segment (4 bytes)
 *
 ****************************************************************************/

ACPI_STATUS
AmlDoSeg (
    char                    *NameString,
    OPERATING_MODE          LoadExecMode)
{
    ACPI_STATUS             Status = AE_OK;
    INT32                   Index;
    char                    CharBuf[5];


    FUNCTION_TRACE ("AmlDoSeg");


    /* If first character is a digit, we aren't looking at a valid name segment    */

    CharBuf[0] = AmlPeek ();

    if ('0' <= CharBuf[0] && CharBuf[0] <= '9')
    {
        DEBUG_PRINT (ACPI_ERROR, ("AmlDoSeg: leading digit: %c\n", CharBuf[0]));
        Status = AE_PENDING;
    }

    else 
    {
        DEBUG_PRINT (TRACE_LOAD, ("AmlDoSeg: Bytes from stream:\n"));

        for (Index = 4; Index > 0 && AmlGoodChar (AmlPeek ()); --Index)
        {
            AmlConsumeStreamBytes (1, (UINT8 *) &CharBuf[4 - Index]);
            DEBUG_PRINT (TRACE_LOAD, ("%c\n", CharBuf[4 - Index]));
        }
    }


    if (AE_OK == Status)
    {
        /* Valid name segment  */

        if (0 == Index)
        {
            /* Found 4 valid characters */
        
            CharBuf[4] = '\0';

            if (NameString)
            {
                strcat (NameString, CharBuf);
                DEBUG_PRINT (TRACE_NAMES, ("AmlDoSeg: Appended to - %s \n", NameString));
            }

            else
            {
                DEBUG_PRINT (TRACE_NAMES, ("AmlDoSeg: No Name string - %s \n", CharBuf));
            }
        }

        else if (4 == Index)
        {
            /* 
             * First character was not a valid name character,
             * so we are looking at something other than a name.
             */
            DEBUG_PRINT (ACPI_INFO, ("AmlDoSeg: Leading char not alpha: %02Xh (not a name)\n", CharBuf[0]));
            Status = AE_PENDING;
        }

        else
        {
            /* Segment started with one or more valid characters, but fewer than 4 */
        
            Status = AE_AML_ERROR;
            DEBUG_PRINT (TRACE_LOAD, ("AmlDoSeg: Bad char %02x in name\n", AmlPeek ()));

            if (MODE_Load == LoadExecMode)
            {
                /* Second pass load mode   */

                REPORT_ERROR ("During LOAD this segment started with one or more valid characters, but fewer than 4");
            }

            else
            {
                DEBUG_PRINT (ACPI_ERROR, ("AmlDoSeg: Bad char %02x in name\n", AmlPeek ()));
            }
        }   
    }

    DEBUG_PRINT (TRACE_EXEC, ("Leave AmlDoSeg %s \n", ExceptionNames[Status]));

    return_ACPI_STATUS (Status);
}


/*****************************************************************************
 *
 * FUNCTION:    AmlDoName
 *
 * PARAMETERS:  DataType            - Data type to be associated with this name
 *              LoadExecMode        - MODE_Load or MODE_Exec
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Print a name, including any prefixes, enter it in the
 *              name space, and put its handle on the stack.
 *
 ****************************************************************************/

ACPI_STATUS
AmlDoName (
    ACPI_OBJECT_TYPE        DataType, 
    OPERATING_MODE          LoadExecMode)
{
    ACPI_STATUS             Status = AE_OK;
    INT32                   NumSegments;
    INT32                   PrefixCount = 0;
    UINT8                   Prefix = 0;
    ACPI_HANDLE             Handle;
    INT32                   MethodFlags;
    INT32                   ArgCount;
    INT32                   PreviousStackTop = 0;
    INT32                   CurrentStackTop = 0;
    UINT32                  StackOffset;
    METHOD_INFO             *MethodPtr;
    ACPI_HANDLE             MethodScope;
    char                    *NameString = NULL;


    FUNCTION_TRACE ("AmlDoName");


BREAKPOINT3;

    if (TYPE_DefField == DataType    || 
        TYPE_BankField == DataType   || 
        TYPE_IndexField == DataType)
    {   
        /* Disallow prefixes for types associated with field names */

        NameString = AmlAllocateNameString (0, 1);
        if (!NameString)
        {
            Status = AE_NO_MEMORY;
        }
        else
        {
            Status = AmlDoSeg (NameString, LoadExecMode);
        }
    }

    else
    {   
        /* DataType is not a field name   */

        switch (AmlPeekOp ())
        {   
            /* Examine first character of name for root or parent prefix operators */

        case AML_RootPrefix:

            AmlConsumeStreamBytes (1, &Prefix);
            DEBUG_PRINT (TRACE_LOAD, ("RootPrefix: %x\n", Prefix));

            /* 
             * Remember that we have a RootPrefix --
             * see comment in AmlAllocateNameString()
             */
            PrefixCount = -1;
            break;


        case AML_ParentPrefix:

            do
            {
                AmlConsumeStreamBytes (1, &Prefix);
                DEBUG_PRINT (TRACE_LOAD, ("ParentPrefix: %x\n", Prefix));

                ++PrefixCount;

            } while (AmlPeekOp () == AML_ParentPrefix);
            
            break;


        default:

            break;
        }



        switch (AmlPeekOp ())
        {
            /* Examine first character of name for name segment prefix operator */
            
        case AML_DualNamePrefix:

            AmlConsumeStreamBytes (1, &Prefix);
            DEBUG_PRINT (TRACE_LOAD, ("DualNamePrefix: %x\n", Prefix));

            NameString = AmlAllocateNameString (PrefixCount, 2);
            if (!NameString)
            {
                Status = AE_NO_MEMORY;
                break;
            }

            /* Ensure PrefixCount != 0 to remember processing a prefix */
            
            PrefixCount += 2;

            if ((Status = AmlDoSeg (NameString, LoadExecMode)) == AE_OK)
            {
                Status = AmlDoSeg (NameString, LoadExecMode);
            }

            break;


        case AML_MultiNamePrefixOp:

            AmlConsumeStreamBytes (1, &Prefix);
            DEBUG_PRINT (TRACE_LOAD, ("MultiNamePrefix: %x\n", Prefix));

            NumSegments = AmlPeek ();                      /* fetch count of segments */

            if (AmlDoByteConst (MODE_Load, 0) != AE_OK)
            {
                /* Unexpected end of AML */

                REPORT_ERROR ("*UNEXPECTED END* [Name]");
                
                Status = AE_AML_ERROR;
                break;
            }

            NameString = AmlAllocateNameString (PrefixCount, NumSegments);
            if (!NameString)
            {
                Status = AE_NO_MEMORY;
                break;
            }

            /* Ensure PrefixCount != 0 to remember processing a prefix */
            
            PrefixCount += 2;

            while (NumSegments && (Status = AmlDoSeg (NameString, LoadExecMode)) == AE_OK)
            {
                --NumSegments;
            }

            break;


        case 0: 
            
            /* NullName valid as of 8-12-98 ASL/AML Grammar Update */
            
            if (-1 == PrefixCount)  
            {
                /* RootPrefix followed by NULL */
            
                DEBUG_PRINT (TRACE_EXEC,
                                ("AmlDoName: NameSeg is \"\\\" followed by NULL\n"));
            }

            AmlConsumeStreamBytes (1, NULL);    /*  consume NULL byte   */
            NameString = AmlAllocateNameString (PrefixCount, 0);
            if (!NameString)
            {
                Status = AE_NO_MEMORY;
                break;
            }

            break;


        default:

            /* Name segment string */

            NameString = AmlAllocateNameString (PrefixCount, 1);
            if (!NameString)
            {
                Status = AE_NO_MEMORY;
                break;
            }

            Status = AmlDoSeg (NameString, LoadExecMode);
            break;

        }   /* Switch (PeekOp ())    */
    }


    if (AE_OK == Status)
    {
        /* All prefixes have been handled, and the name is in NameString */

        AmlObjStackDeleteValue (STACK_TOP);

        Status = NsLookup (CurrentScope->Scope, NameString, DataType, LoadExecMode, 
                                    NS_SEARCH_PARENT, (NAME_TABLE_ENTRY **) AmlObjStackGetTopPtr ());

        /* Get return value from the lookup */

        Handle = AmlObjStackGetValue (STACK_TOP);

        /* TBD: another global to remove!! */
        /* Globally set this handle for use later */

        if (MODE_Load1 == LoadExecMode)
        {
            LastMethod = Handle;
        }

        if (MODE_Exec == LoadExecMode && !Handle)
        {
            DEBUG_PRINT (ACPI_ERROR, ("AmlDoName: Name Lookup Failure\n"));
            Status = AE_AML_ERROR;
        }

        else if (MODE_Load1 != LoadExecMode)
        {   
            /* Not first pass load */

            if (TYPE_Any == DataType && 
                TYPE_Method == NsGetType (Handle))
            {   
                /* 
                 * Method reference call 
                 * The name just looked up is a Method that was already
                 * defined, so this is a reference (call).  Scan the args.
                 * The arg count is in the MethodFlags, which is the first
                 * byte of the Method's AML.
                 */

                MethodPtr = (METHOD_INFO *) NsGetValue (Handle);
                if (MethodPtr)
                {   
                    /* MethodPtr valid   */
                    
                    MethodFlags = AmlGetPCodeByte (MethodPtr->Offset);

                    if (AML_END_OF_BLOCK == MethodFlags)
                    {
                        DEBUG_PRINT (ACPI_ERROR, ("AmlDoName: invoked Method %s has no AML\n",
                                        NameString));
                        Status = AE_AML_ERROR;
                    }

                    else
                    {   
                        /* MethodPtr points at valid method  */
                        
                        ArgCount = (MethodFlags & METHOD_ARG_COUNT_MASK) >> METHOD_ARG_COUNT_SHIFT;

                        PreviousStackTop = AmlObjStackLevel ();
                        MethodScope = AmlObjStackGetValue (STACK_TOP);

                        if (((Status = AmlObjStackPushIfExec (MODE_Exec)) == AE_OK) &&
                             (ArgCount > 0))
                        {   
                            /* Get all arguments */

                            while (ArgCount-- && (AE_OK == Status))
                            {   
                                /* Get each argument */
                                
                                if (AE_OK == (Status = AmlDoOpCode (LoadExecMode)))
                                {   
                                    /* Argument is now on the object stack */
                                    
                                    /*  
                                     * Arguments (e.g., local variables and control
                                     * method arguments) passed to control methods
                                     * are values, not references.
                                     */
                                    
                                    /*
                                     * TBD: RefOf problem with AmlGetRvalue() conversion.
                                     */
                                    if (MODE_Exec == LoadExecMode)
                                    {
                                        Status = AmlGetRvalue (AmlObjStackGetTopPtr ());
                                    }

                                    if (AE_OK == Status)
                                    {
                                        /* Make room for the next argument */

                                        Status = AmlObjStackPushIfExec (LoadExecMode);
                                    }
                                } 
                            }
                        } 

                        if ((AE_OK == Status) && (MODE_Exec == LoadExecMode))
                        {   
                            /* Execution mode  */
                            /* Mark end of arg list */

                            AmlObjStackDeleteValue (STACK_TOP);

                            /* Establish Method's scope as current */

                            NsPushMethodScope (MethodScope);
                            CurrentStackTop = AmlObjStackLevel ();
                            StackOffset = CurrentStackTop - PreviousStackTop;

                            DEBUG_PRINT (TRACE_LOAD, ("Calling %4.4s, PreviousTOS=%d  CurrentTOS=%d\n",
                                            MethodScope, PreviousStackTop, CurrentStackTop));

                            AmlDumpObjStack (LoadExecMode, "AmlDoName", ACPI_INT_MAX, "Method Arguments");

                            /* Execute the Method, passing the stacked args */
                            
                            Status = AmlExecuteMethod (MethodPtr->Offset + 1, MethodPtr->Length - 1,
                                                        AmlObjStackGetPtr (StackOffset -1));

                            CurrentStackTop = AmlObjStackLevel ();

                            DEBUG_PRINT (TRACE_LOAD, ("After AmlExecuteMethod, PreviousTOS=%d  CurrentTOS=%d\n",
                                            PreviousStackTop, CurrentStackTop));

                            if (AE_RETURN_VALUE == Status)
                            {
                                /* 
                                 * Move the returned value from the top of the stack to
                                 * below the method args.
                                 */

                                if (PreviousStackTop < CurrentStackTop)
                                {
                                    StackOffset = CurrentStackTop - PreviousStackTop;

                                    AmlObjStackDeleteValue (StackOffset);
                                    AmlObjStackSetValue (StackOffset, AmlObjStackGetValue (STACK_TOP));
                                    AmlObjStackPop (1);
                                }

                                Status = AE_OK;
                            }

                            /* Pop scope stack */
                            
                            NsPopCurrent (TYPE_Any);

                        } /* Execution mode  */


                        /* Clean up object stack */
                        
                        DEBUG_PRINT (TRACE_LOAD, ("AmlDoName: Cleaning up object stack (%d elements)\n",
                                        CurrentStackTop - PreviousStackTop));

                        for (CurrentStackTop = AmlObjStackLevel (); 
                             CurrentStackTop > PreviousStackTop;
                             CurrentStackTop--)
                        {
                            AmlObjStackDeleteValue (STACK_TOP);

                            /* Zero out the slot and move on */

                            AmlObjStackPop (1);
                        }

                    } /* Else - valid method ptr */

                } /* Has a non-zero method ptr */

            } /* Reference to a control method */

        } /* Second pass load or execute mode */
    
    } /* Status AE_OK */


    else if (AE_PENDING == Status && PrefixCount != 0)
    {
        /* Ran out of segments after processing a prefix */

        if (MODE_Load1 == LoadExecMode || MODE_Load == LoadExecMode)
        {
            DEBUG_PRINT (ACPI_ERROR, ("AmlDoName: ***Malformed Name***\n"));

            REPORT_ERROR ("Ran out of segments after processing a prefix");
        }

        else
        {
            DEBUG_PRINT (ACPI_ERROR, ("AmlDoName: Malformed Name\n"));
        }

        Status = AE_AML_ERROR;
    }


    /* Cleanup */

    if (NameString)
    {
        CmFree (NameString);
    }

    return_ACPI_STATUS (Status);
}



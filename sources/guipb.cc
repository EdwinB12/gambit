
//
// FILE: guipb.cc -- the GUI playback class.
//


#include "glist.h"
#include "guirecdb.h"
#include "guipb.h"

#include "stdlib.h"

#define MAX_LINELENGTH 128  // FIXME: This should really be somewhere else.


// The global GuiPlayback object:
GuiPlayback gui_playback;

// Initialize static class members.
bool GuiPlayback::instantiated = false;


// ----------------------------------------------------------------------
//                  string utilities
// ----------------------------------------------------------------------

gText strip_whitespace(const gText& s)
{
    // Remove leading and trailing whitespace from a string s
    // and return the result.

    if (s.Length() == 0)
        return s;

    int first = 0;            // first non-whitespace character
    int last  = s.Length();   // one past the last non-whitespace character

    // Find first non-whitespace character.

    for (int i = 0; i < s.Length(); i++)
    {
        if (s[i] == ' ' || s[i] == '\t')
            first++;
        else // non-whitespace
            break;
    }

    // Find last non-whitespace character.  
    // "Whitespace" here includes newlines.

    for (int i = s.Length() - 1; i >= 0; i--)
    {
        if (s[i] == ' ' || s[i] == '\t' || s[i] == '\n')
            last--;
        else
            break;
    }

    int len = last - first;

    if (last <= first) // no non-whitespace characters in string
    {
        return gText("");
    }

    // We have to add 1 to first because the Mid function subtracts 1, 
    // for some stupid reason.
    gText out = s.Mid(len, first + 1);

    return out;
}



// ----------------------------------------------------------------------
//                  GuiPlayback exception classes
// ----------------------------------------------------------------------

gText GuiPlayback::FileNotFound::Description(void) const
{
    return "The log file was not found.";
}


gText GuiPlayback::InUse::Description(void) const
{
    return "A GuiPlayback object is already in use.";
}


gText GuiPlayback::InvalidInputLine::Description(void) const
{
    return "An invalid input line was read from the log file.";
}


gText GuiPlayback::InvalidInputField::Description(void) const
{
    return "An invalid input field was read from the log file.";
}


gText GuiPlayback::ObjectNotFound::Description(void) const
{
    return "A GUI object from the log file was not found in the database.";
}


gText GuiPlayback::UnknownObjectType::Description(void) const
{
    return "Unknown GUI object type in log file.";
}


gText GuiPlayback::InvalidCommandForObject::Description(void) const
{
    return "Invalid command for object type in log file.";
}


// ----------------------------------------------------------------------
//                        GuiPlayback methods
// ----------------------------------------------------------------------

// Constructor, destructor.

GuiPlayback::GuiPlayback()
{
    // Is there an instance already?  If so, abort.
    if (instantiated)
    {
        throw InUse();
    }

    fp = NULL;
    instantiated = true;
}


GuiPlayback::~GuiPlayback()
{ 
    if (fp != NULL)
    {
        fclose(fp);
    }

    instantiated = false;
}


// Playback methods.

void GuiPlayback::Playback(const gText& filename)
{
#ifdef GUIPB_DEBUG
    printf("playing back file: %s\n", (char *)filename);
#endif

    char linebuf[MAX_LINELENGTH];
    gText line;

    fp = fopen((char *)filename, "r");

    if (fp == NULL)
    {
        throw FileNotFound();
    }

    // Read in the file line-by-line and execute each line.

    while(1)
    {
        char *result = fgets(linebuf, MAX_LINELENGTH, fp);

        if (result == NULL) // End of file or error.
            break;

        line = linebuf;
        PlaybackLine(line);
    }
}


void GuiPlayback::PlaybackLine(const gText& line)
{
#ifdef GUIPB_DEBUG
    // Note that line ends with a newline so we don't
    // put another newline in here.
    printf("playing back line: %s", (char *)line);
#endif

    // -------------------------------------------------------
    // If the line is empty (just a newline), do nothing.
    // Otherwise, parse the line.  It should have the form:
    // OBJECT#INSTANCE_NUMBER, COMMAND [, arg1, arg2, ...]
    // -------------------------------------------------------

    // Low-level variables.
    char  linebuf[MAX_LINELENGTH];
    char *word;

    // High-level variables.
    gText object_name;
    gText class_name;
    gText command;
    gText argument;
    gList<gText> arglist;

    strcpy(linebuf, (char *)line);

    if (strcmp(linebuf, "\n") == 0)
        return;  // Empty line.

    // -------------------------------------------------------
    // Process the line.
    // The line consists of comma-separated fields.
    // -------------------------------------------------------

    assert(linebuf != NULL);

    // First, get the name of the object.

    word = strtok(linebuf, ",");

    if (word == NULL)  // No tokens found.
        throw InvalidInputLine();

    object_name = strip_whitespace(gText(word));

    if (object_name.Length() <= 0)
        throw InvalidInputField();

#ifdef GUIPB_DEBUG
    printf("object: %s\n", (char *)object_name);
#endif

    // Then get the command.

    word = strtok(NULL, ",");

    if (word == NULL)  // No more tokens.
        throw InvalidInputLine();

    command = strip_whitespace(gText(word));

    if (command.Length() <= 0)
        throw InvalidInputField();

#ifdef GUIPB_DEBUG
    printf("command: %s\n", (char *)command);
#endif

    // Get the list of arguments, if any.

    while(1)
    {
        word = strtok(NULL, ",");

        if (word == NULL)
            break;  // No more arguments.

        argument = strip_whitespace(gText(word));
        arglist.Append(argument);

#ifdef GUIPB_DEBUG
        printf("argument: %s\n", (char *)argument);
#endif
    }

    // Now execute the command.

    ExecuteCommand(object_name, command, arglist);
}



void GuiPlayback::ExecuteCommand(const gText& object_name, 
                                 const gText& command,
                                 const gList<gText>& arglist)
{
#ifdef GUIPB_DEBUG
    printf("in GuiPlayback::ExecuteCommand...\n");

    printf("object_name: %s\n", (char *)object_name);
    printf("command: %s\n", (char *)command);

    for (int i = 1; i <= arglist.Length(); i++)
        printf("arglist[%d] = %s\n", i, (char *)arglist[i]);
#endif

    // Check that the object actually exists in the (global) database.
    // WARNING! Hash class both doesn't do proper error handling and
    //          returns an "illegal value" which is never set anywhere,
    //          can't be checked as a result, and can't be overridden 
    //          by subclasses!  FIXME!

    if (gui_recorder_db.IsDefined(object_name) == 0) // This shouldn't be necessary.
        throw ObjectNotFound();

    GuiObject *object = gui_recorder_db(object_name);

    // Separate the object name from the object number.  The object name is
    // whatever comes before the last '#' and the number is whatever comes
    // after.  The object name should never contain a '#' character.

    // FIXME! Must make a local copy of object_name because LastOccur isn't
    // a const function!

    gText obj(object_name);  
    int length   = obj.Length();
    int position = obj.LastOccur('#');

    if (position == -1) // '#' not found
    {
        throw InvalidInputField();
    }

    position--;  // Move back behind the '#'.

    assert((length >= 0) && (position >= 0) && (position <= length));

    gText object_type = obj.Left(position);                // CHECKME: memory leak?
    gText num         = obj.Right(length - position - 1);  // CHECKME: memory leak?

#ifdef GUIPB_DEBUG
    int number = atoi((char *)num);
    printf("object type: %s\n", (char *)object_type);
    printf("number: %d\n", number);
#endif

    // Cast the object to the correct type.  Check that the object is
    // indeed what it claims to be.  Call the function appropriate for the
    // object with the command and arglist as arguments.

    // FIXME! Add lots of "else if" clauses here as I extend it...
    if (object_type == "GambitFrame")
    {
#ifdef GUIPB_DEBUG
        printf("object type found: %s\n", (char *)object_type);
#endif 

        GambitFrame *gambit_frame = (GambitFrame *)(object->get_object());
        assert(gambit_frame->is_GambitFrame());
#ifdef GUIPB_DEBUG
        gambit_frame->GambitFrame_hello();
#endif

        ExecuteGambitFrameCommand(gambit_frame, command, arglist);
    }
    else if (object_type == "EfgShow")
    {
#ifdef GUIPB_DEBUG
        printf("object type found: %s\n", (char *)object_type);
#endif 

        EfgShow *efg_show = (EfgShow *)(object->get_object());
        assert(efg_show->is_EfgShow());
#ifdef GUIPB_DEBUG
        efg_show->EfgShow_hello();
#endif

        ExecuteEfgShowCommand(efg_show, command, arglist);
    }
    else if (object_type == "SpreadSheet3D")
    {
#ifdef GUIPB_DEBUG
        printf("object type found: %s\n", (char *)object_type);
#endif 

        SpreadSheet3D *spreadsheet3d = (SpreadSheet3D *)(object->get_object());
        assert(spreadsheet3d->is_SpreadSheet3D());
#ifdef GUIPB_DEBUG
        spreadsheet3d->SpreadSheet3D_hello();
#endif

        ExecuteSpreadSheet3DCommand(spreadsheet3d, command, arglist);
    }
    else  // The object is not of a loggable class.
    {
        throw UnknownObjectType();
    }
}


// Debugging functions.

bool GuiPlayback::is_GuiPlayback() const
{
    return true;
}


void GuiPlayback::GuiPlayback_hello() const
{
    printf("instance of class GuiPlayback accessed at %x\n", 
           (unsigned int)this);
}


// ============================================================
//
//       Object-specific functions.
//
// NOTE: it may be better in the long run to farm these out to
//       their classes.
//
// ============================================================

// GambitFrame class

void GuiPlayback::ExecuteGambitFrameCommand(GambitFrame *object,
                                            const gText& command,
                                            const gList<gText>& arglist)
{
#ifdef GUIPB_DEBUG
    printf("in GuiPlayback::ExecuteGambitFrameCommand...\n");
    printf("command: %s\n", (char *)command);

    for (int i = 1; i <= arglist.Length(); i++)
        printf("arglist[%d] = %s\n", i, (char *)arglist[i]);
#endif

    // FIXME! add commands.

    if (command == "FILE:QUIT")
    {
        object->Close();
    }
    else if (command == "FILE:LOAD")
    {
        object->LoadFile((char *)arglist[1]);
    }
    else
    {
        throw InvalidCommandForObject();
    }
}


// EfgShow class

void GuiPlayback::ExecuteEfgShowCommand(EfgShow *object,
                                        const gText& command,
                                        const gList<gText>& arglist)
{
#ifdef GUIPB_DEBUG
    printf("in GuiPlayback::ExecuteEfgShowCommand...\n");
    printf("command: %s\n", (char *)command);
    
    for (int i = 1; i <= arglist.Length(); i++)
        printf("arglist[%d] = %s\n", i, (char *)arglist[i]);
#endif
    
    // FIXME! add commands.
    
    if (command == "SOLVE:SOLVE")
    {
        object->Solve();
    }
    else
    {
        throw InvalidCommandForObject();
    }
}


// SpreadSheet3D class

void GuiPlayback::ExecuteSpreadSheet3DCommand(SpreadSheet3D *object,
                                              const gText& command,
                                              const gList<gText>& arglist)
{
#ifdef GUIPB_DEBUG
    printf("in GuiPlayback::ExecuteEfgShowCommand...\n");
    printf("command: %s\n", (char *)command);
    
    for (int i = 1; i <= arglist.Length(); i++)
        printf("arglist[%d] = %s\n", i, (char *)arglist[i]);
#endif
    
    // FIXME! add commands.
    
    if (command == "PRINT")
    {
        object->OnPrint_Playback((char *)arglist[1], (char *)arglist[2]);
    }
    else
    {
        throw InvalidCommandForObject();
    }
}

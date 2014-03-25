

#include <cstdio>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <fstream>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

using namespace std;

 /* reverse:  reverse string s in place */
void
reverse (char s[])
{
  int i, j;
  char c;

  for (i = 0, j = strlen (s) - 1; i < j; i++, j--)
    {
      c = s[i];
      s[i] = s[j];
      s[j] = c;
    }
}

/* converts n to its inversed corresponding hex string (filled with 0, but no 0x prefix)*/
void
uint64_t_to_hex (uint64_t n, char s[])
{
  int i;
  i = 0;

  do
    {				/* generate digits in reverse order */
      if (n % 16 > 9)
	{
	  s[i++] = n % 16 + 'A' - 10;
	}
      else
	{
	  s[i++] = n % 16 + '0';	/* get next digit */
	}
    }
  while ((n /= 16) > 0);	/* delete it */

  while (i < 16)
    {
      s[i++] = '0';
    }

  s[i] = '\0';
  reverse (s);
}



int
main (int argc, char *argv[])
{
  int i = 0;
  uint32_t read_value[2] = { 0 };
  uint64_t media_id = 0;
  char temp_char = '\0';
  char *char_pointer = NULL;
  char save_name[13] = { '\0' };
  const char *const magic = "NCSD";
  ifstream CCI;

  if (2 != argc)
    {
      cout << "Needs CCI filename as only argument." << endl;
      exit (EXIT_FAILURE);
    }

  char_pointer = strrchr (argv[1], '.');
  if (NULL == char_pointer)
    {
      cout << argv[1] << " doesn't have a \'.\' seperated ending." << endl;
      exit (EXIT_FAILURE);
    }

  CCI.open (argv[1], ios::in | ios::binary);
  if (!CCI.is_open ())
    {
      cout << "Failed to open " << argv[1] << endl;
      exit (EXIT_FAILURE);
    }

  CCI.seekg (0x100);		//should contain "NCSD" (small sanity check)

  for (int i = 0; i < 4; ++i)
    {
      CCI.read (&temp_char, 1);
      if (magic[i] != temp_char)
	{
	  cout << "Magic not found." << argv[1] << " is no valid CCI." <<
	    endl;
	  exit (EXIT_FAILURE);
	}
    }

  CCI.seekg (0x108);		//media id offset  


  if (CCI.read ((char *) read_value, sizeof (read_value)))
    {
      media_id =
	(uint64_t) (read_value[1]) << 32 | (uint64_t) (read_value[0]);

    }

  CCI.close ();

  uint64_t_to_hex (media_id, save_name);
  strcat (save_name, ".sav");

//at this point char_pointer points to the last '.' of argv[1]. and yeah, i was too lazy for propper substring replacement
  char_pointer++;
  *(char_pointer++) = 's';
  *(char_pointer++) = 'a';
  *(char_pointer++) = 'v';

  if (0 != rename (save_name, argv[1]))
    {
      cout << "Error renaming file " << save_name << " to " << argv[1] << flush;
      perror (" ");
      cout << endl;
      exit (EXIT_FAILURE);
    }

  return EXIT_SUCCESS;
}

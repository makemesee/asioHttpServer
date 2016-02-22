#include <string>

struct MIMETab
{
    const char* extension;
    const char* mimeType;
} mappings[] =
{
    { "gif", "image/gif" },
    { "htm", "text/html" },
    { "html", "text/html" },
    { "jpg", "image/jpeg" },
    { "png", "image/png" },
    { 0, 0 } // Метка конца списка
};

std::string ExtensionToMIME(const std::string &extension)
{
	for (MIMETab *m = mappings; m->extension; ++m)
	{
		if (m->extension == extension)
		{
			return m->mimeType;
		}
	}

	return "text/plain";
}


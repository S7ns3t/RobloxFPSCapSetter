#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;


int main()
{
	char* buffer = nullptr;
	size_t bufferSize = 0;
	if (_dupenv_s(&buffer, &bufferSize, "USERPROFILE") == 0 && buffer != nullptr)  // In retrospect, I should've inverted if's for the sake of less nesting
	{
		fs::directory_iterator robloxVersions = fs::directory_iterator((std::string)buffer + "\\AppData\\Local\\Roblox\\Versions");
		robloxVersions++; // Skip RobloxStudioInstaller.exe
		for (const fs::directory_entry& version : robloxVersions)
		{
			std::string versionPathString = version.path().string();
			if (fs::exists(fs::path(versionPathString + "\\RobloxPlayerBeta.exe")))
			{
				if (!fs::exists(fs::path(versionPathString + "\\ClientSettings")))
				{
					if (!fs::create_directory(fs::path(versionPathString + "\\ClientSettings")))
					{
						std::cout << "ERROR: Failed to open ClientAppSettings.json" << std::endl;
						return -3;
					}
				}

				FILE* clientAppSettings;
				fopen_s(&clientAppSettings, (versionPathString + "\\ClientSettings\\ClientAppSettings.json").c_str(), "w");
				if (clientAppSettings)
				{
					int newTargetFPS;
					std::cout << "Enter new FPS cap (will stay persistent until roblox update): ";
					std::cin >> newTargetFPS;

					if (fputs(("{ \"DFIntTaskSchedulerTargetFps\": " + std::to_string(newTargetFPS) + " }").c_str(), clientAppSettings) != EOF)
					{
						std::cout << "Successfully set FPS cap to " << newTargetFPS << std::endl;
						fclose(clientAppSettings);
					}
					else
					{
						std::cout << "ERROR: Failed to write to ClientAppSettings.json" << std::endl;
						return -4;
					}
				}
				else
				{
					std::cout << "ERROR: Failed to create ClientSettings directory" << std::endl;
					return -2;
				}
				break;
			}
		}
	}
	else
	{
		std::cout << "ERROR: Failed to grab user's directory" << std::endl;
		return -1;
	}

	free(buffer);

	std::cout << "\nPress any key to continue..."; std::cin.get();
	return 0;
}
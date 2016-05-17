#!/bin/bash
installation_folder="/Library/gpu-switch_hooks/"
hooks_folder="hooks"
loginhook="gpu-switch_loginhook"
logouthook="gpu-switch_logouthook"
binary="gpu-switch"
binary_folder="build"
#-- Directory containing this installer and the scripts to install.
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$(id -u)" != "0" ]; then
	printf "You need administrative privileges to install this script.\nPlease run: sudo ./install_hooks.sh\n"
	exit 1
fi

uninstallmode=false

while getopts ":u" opt; do
  case $opt in
    u)
		uninstallmode=true
      ;;
    \?)
      printf "Invalid option: -$OPTARG\n" >&2
      ;;
  esac
done


if [ "$uninstallmode" = true ]; then
	printf "Removing hooks...\n"
	defaults delete com.apple.loginwindow LoginHook
	defaults delete com.apple.loginwindow LogoutHook
	
	printf "Removing files...\n"
	rm "$installation_folder$loginhook"
	rm "$installation_folder$logouthook"
    rm "$installation_folder$binary"
    rm -rf "$installation_folder"
	printf "Done!\n"
	
else
    printf "Building gpu-switch\n"
    # Create the tempoaray build folder
    # rm -rf "${DIR}/$binary_folder"
    # mkdir -p "${DIR}/$binary_folder"
    # xcodebuild -project "${DIR}/gpu-switch.xcodeproj" -scheme gpu-switch -configuration Release -derivedDataPath "${DIR}/$binary_folder" | egrep '^(/.+:[0-9+:[0-9]+:.(error):|fatal|===)' -
    sh "${DIR}/build.sh"

	printf "Copying files...\n"

	# Create installation folder if it doesn't already exists.
	mkdir -p "$installation_folder"

	# Copy login and logout scripts and make them executable
	sudo cp "${DIR}/$hooks_folder/$loginhook" "$installation_folder"
	sudo cp "${DIR}/$hooks_folder/$logouthook" "$installation_folder"
	sudo chmod +x "$installation_folder$loginhook"
	sudo chmod +x "$installation_folder$logouthook"

    # Copy the binary
    # sudo cp "${DIR}/$binary_folder/Build/Products/Release/$binary" "$installation_folder"
    sudo cp "${DIR}/$binary_folder/$binary" "$installation_folder"

	printf "Registering hooks...\n"
	# Register the scripts as login and logout hooks
	defaults write com.apple.loginwindow LoginHook  "$installation_folder$loginhook"
	defaults write com.apple.loginwindow LogoutHook "$installation_folder$logouthook"

    printf "Cleaning Up...\n"
    rm -rf "${DIR}/$binary_folder"

	printf "Done!\n"
fi

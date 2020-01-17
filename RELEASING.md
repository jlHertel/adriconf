## Doing a release

To do a release follow those steps:

1. Update the [VERSION](VERSION) file with the newest number.
2. Update the flatpak release notes in the
 flatpak xml [file](flatpak/br.com.jeanhertel.adriconf.appdata.xml).
3. Update the flatpak json version [file](flatpak/br.com.jeanhertel.adriconf.json).
4. Do a Github release/tagging of the code.
You can copy-paste the release notes you wrote in the flatpak file to Github.
**Any code available on MASTER branch should be released.**
5. Now go to adriconf Flathub [repository][1] and update the release notes
and json files to use the release you just made on Github. 
6. Commit the changes and wait for Flathub bot to build the new version.

After doing a release Flathub usually takes some hours to publish the version.

[1]: https://github.com/flathub/br.com.jeanhertel.adriconf
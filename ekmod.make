; $Id$
; Example drush make file for downloading third party EKMOD library.
; http://bit.ly/EMVW5

core = 7.x
api = 2
projects[] = drupal

libraries[ekmod][download][type] = "post"
libraries[ekmod][download][post_data] = "format=tar&adapter=jquery&players[]=img&players[]=iframe&players[]=html&players[]=swf&players[]=flv&players[]=qt&players[]=wmp&language=en&css_support=on"
libraries[ekmod][download][file_type] = "tar.gz"
libraries[ekmod][download][url] = ""
libraries[ekmod][directory_name] = "ekmod"
libraries[ekmod][destination] = "libraries"

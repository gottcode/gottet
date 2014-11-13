#!/bin/sh


echo -n 'Preparing files...'
cd ..

rm -f gottet.desktop.in.h
rm -f gottet.desktop.in
cp gottet.desktop gottet.desktop.in
sed -e '/^Name\[/ d' \
	-e '/^GenericName\[/ d' \
	-e '/^Comment\[/ d' \
	-e 's/^Name/_Name/' \
	-e 's/^GenericName/_GenericName/' \
	-e 's/^Comment/_Comment/' \
	-i gottet.desktop.in
intltool-extract --quiet --type=gettext/ini gottet.desktop.in

rm -f gottet.appdata.xml.in.h
rm -f gottet.appdata.xml.in
cp gottet.appdata.xml gottet.appdata.xml.in
sed -e '/p xml:lang/ d' \
	-e '/summary xml:lang/ d' \
	-e 's/<p>/<_p>/' \
	-e 's/<\/p>/<\/_p>/' \
	-e 's/<summary>/<_summary>/' \
	-e 's/<\/summary>/<\/_summary>/' \
	-i gottet.appdata.xml.in
intltool-extract --quiet --type=gettext/xml gottet.appdata.xml.in

cd po
echo ' DONE'


echo -n 'Extracting messages...'
xgettext --from-code=UTF-8 --c++ --keyword=_ --keyword=N_:1 \
	--package-name='Gottet' --copyright-holder='Graeme Gott' \
	--output=description.pot ../*.h
sed 's/CHARSET/UTF-8/' -i description.pot
echo ' DONE'


echo -n 'Updating translations...'
for POFILE in *.po;
do
	echo -n " $POFILE"
	msgmerge --quiet --update --backup=none $POFILE description.pot
done
echo ' DONE'


echo -n 'Merging translations...'
cd ..

intltool-merge --quiet --desktop-style po gottet.desktop.in gottet.desktop
rm -f gottet.desktop.in.h
rm -f gottet.desktop.in

intltool-merge --quiet --xml-style po gottet.appdata.xml.in gottet.appdata.xml
rm -f gottet.appdata.xml.in.h
rm -f gottet.appdata.xml.in

echo ' DONE'

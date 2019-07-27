## Macro=[[Vorlage(Fremdquelle-auth, %%URL zum PGP-Key | key PGP-Schlüsselnummer%%)]]
Um die [:Fremdquellen: Fremdquelle] zu authentifizieren, kann man <@ if $arguments.0 == 'key' @> den Signierungsschlüssel mit folgendem Befehl importieren:
{{{#!vorlage Befehl
sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com <@ $arguments.1 @> 
}}}
<@ else @>
entweder den [<@ $arguments.0 @> Signierungsschlüssel herunterladen] {dl} und in der [:Paketquellen_freischalten: Paketverwaltung hinzufügen] oder folgenden Befehl ausführen:
{{{#!vorlage Befehl
wget -q -O "-" <@ $arguments.0 @> | sudo apt-key add -
}}}
<@ endif @>

## Macro=[[Vorlage(OBS, %%OBS Repository%%, xUbuntu)]]
Um das Paket aus einer vom [:Open Build Service:] generierten Paketquelle zu installieren, muss man zuerst die [:Paketquellen_freischalten:Paketquelle freischalten], indem man folgenden Befehl mit [:root:root-Rechten] im [:Terminal:] ausführt:

{{|<class="thirdpartyrepo-outer
<@ if $arguments as lowercase contain 'precise' @> thirdpartyrepo-version-12.04
<@ endif @>
<@if $arguments as lowercase contain 'trusty' @> thirdpartyrepo-version-14.04
<@ endif @>
<@if $arguments as lowercase contain 'wily' @> thirdpartyrepo-version-15.10
<@ endif @>">
{{{#!vorlage Befehl
sudo add-apt-repository 'deb http://download.opensuse.org/repositories/<@ $arguments.0 @>/<@ $arguments.1 @>_VERSION/ /'
}}}
|}}

[[Vorlage(Fremd, Quelle)]] 

Anschließend sollte die [:Fremdquelle:] authentifiziert werden. Dazu lädt man sich mit dem folgenden Befehlen den benötigten Schlüssel herunter und fügt diesen dem Schlüsselbund hinzu:
{{|<class="thirdpartyrepo-outer
<@ if $arguments as lowercase contain 'precise' @> thirdpartyrepo-version-12.04
<@ endif @>
<@if $arguments as lowercase contain 'trusty' @> thirdpartyrepo-version-14.04
<@ endif @>
<@if $arguments as lowercase contain 'wily' @> thirdpartyrepo-version-15.10
<@ endif @>">
{{{#!vorlage Befehl
wget http://download.opensuse.org/repositories/<@ $arguments.0 @>/xUbuntu_VERSION/Release.key
sudo apt-key add - < Release.key
}}}
|}}

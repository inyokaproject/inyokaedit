## Macro=[[Vorlage(OBS, %%OBS Repository%%, Ubuntu-Version)]]
Um das Paket aus einer vom [:Open Build Service:] generierten Paketquelle zu installieren, muss man zuerst die [:Paketquellen_freischalten:Paketquelle freischalten], indem man folgenden Befehl mit [:sudo:root-Rechten] im [:Terminal:] ausführt:

{{|<class="thirdpartyrepo-outer
<@ if $arguments as lowercase contain 'focal' @> thirdpartyrepo-version-20.04
<@ endif @>
<@if $arguments as lowercase contain 'jammy' @> thirdpartyrepo-version-22.04
<@ endif @>
<@if $arguments as lowercase contain 'noble' @> thirdpartyrepo-version-24.04
<@ endif @>">
{{{#!vorlage Befehl
sudo cat <EOF > /etc/apt/sources.list.d/<@ $arguments.0 @>.sources
Types: deb 
URIs: http://download.opensuse.org/repositories/<@ $arguments.0 @>/<@ $arguments.1 @>_VERSION/
Suites: /
Signed-By: /etc/apt/keyrings/<@ $arguments.0 @>.gpg
EOF
}}}
|}}

[[Vorlage(Fremd, Quelle)]] 

Anschließend sollte die [:Fremdquelle:] authentifiziert werden. Dazu lädt man sich mit den folgenden Befehlen den benötigten Schlüssel herunter und legt diesen an dem dafür vorgesehenen Ort ab:
{{|<class="thirdpartyrepo-outer
<@ if $arguments as lowercase contain 'focal' @> thirdpartyrepo-version-20.04
<@ endif @>
<@if $arguments as lowercase contain 'jammy' @> thirdpartyrepo-version-22.04
<@ endif @>
<@if $arguments as lowercase contain 'noble' @> thirdpartyrepo-version-24.04
<@ endif @>">
{{{#!vorlage Befehl
wget -q -O - http://download.opensuse.org/repositories/<@ $arguments.0 @>/xUbuntu_VERSION/Release.key | gpg --dearmor | sudo tee /etc/apt/keyrings/<@ $arguments.0 @>.gpg > /dev/null
}}}
|}}

## Macro=[[Vorlage(Fremdquelle, %%URL, Ubuntuversion(en), Komponente(n)%%)]]

Um aus der [:Fremdquellen: Fremdquelle] zu installieren, muss man
<@ if $arguments as lowercase contain 'jammy' or
      $arguments as lowercase contain 'lunar' or
      $arguments as lowercase contain 'focal' or
      $arguments as lowercase contain 'mantic' @> die folgenden [:Paketquellen freischalten:]:
[[Vorlage(Fremd, Quelle)]] 


{{|<class="thirdpartyrepo-outer
<@if $arguments as lowercase contain 'focal' @> thirdpartyrepo-version-focal
<@ endif @>
<@if $arguments as lowercase contain 'mantic' @> thirdpartyrepo-version-lunar
<@ endif @>
<@if $arguments as lowercase contain 'jammy' @> thirdpartyrepo-version-jammy
<@ endif @>
<@if $arguments as lowercase contain 'lunar' @> thirdpartyrepo-version-mantic
<@ endif @>
">
{{{
deb <@ $arguments.0 @> VERSION
<@ for $arg in $arguments @>
<@ if $loop.index > 1 @>
<@ if ['mantic', 'jammy', 'lunar', 'focal'] contains $arg @>
<@ else @> <@ $arg @>
<@ endif @>
<@ endif @>
<@ endfor @>
}}}
|}}


<@ else @> unabhängig von der Ubuntu-Version die folgende [:Paketquellen freischalten: Paketquelle freischalten]:
[[Vorlage(Fremd, Quelle)]]
{{{
deb <@ $arguments join_with ' ' @>
}}}
<@ endif @>

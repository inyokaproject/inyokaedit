## Macro=[[Vorlage(Fremdquelle, %%URL, Ubuntuversion(en), Komponente(n)%%)]]
Um aus der [:Fremdquellen: Fremdquelle] zu installieren, muss man
<@ if $arguments as lowercase contain 'artful' or
      $arguments as lowercase contain 'zesty' or
      $arguments as lowercase contain 'xenial' or
      $arguments as lowercase contain 'trusty' or
      $arguments as lowercase contain 'precise' @> die folgenden [:Paketquellen freischalten:]:
[[Vorlage(Fremd, Quelle)]] 


{{|<class="thirdpartyrepo-outer
<@ if $arguments as lowercase contain 'precise' @> thirdpartyrepo-version-precise
<@ endif @>
<@if $arguments as lowercase contain 'trusty' @> thirdpartyrepo-version-trusty
<@ endif @>
<@if $arguments as lowercase contain 'xenial' @> thirdpartyrepo-version-xenial
<@ endif @>
<@if $arguments as lowercase contain 'zesty' @> thirdpartyrepo-version-zesty
<@ endif @>
<@if $arguments as lowercase contain 'artful' @> thirdpartyrepo-version-artful
<@ endif @>
">
{{{
deb <@ $arguments.0 @> VERSION
<@ for $arg in $arguments @>
<@ if $loop.index > 1 @>
<@ if ['artful', 'zesty', 'xenial', 'trusty', 'precise'] contains $arg @>
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

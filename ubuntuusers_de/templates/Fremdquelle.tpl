## Macro=[[Vorlage(Fremdquelle, %%URL, Ubuntuversion(en), Komponente(n)%%)]]
Um aus der [:Fremdquellen: Fremdquelle] zu installieren, muss man
<@ if $arguments as lowercase contain 'disco' or
      $arguments as lowercase contain 'bionic' or
      $arguments as lowercase contain 'xenial' @> die folgenden [:Paketquellen freischalten:]:
[[Vorlage(Fremd, Quelle)]] 


{{|<class="thirdpartyrepo-outer
<@if $arguments as lowercase contain 'xenial' @> thirdpartyrepo-version-xenial
<@ endif @>
<@if $arguments as lowercase contain 'bionic' @> thirdpartyrepo-version-bionic
<@ endif @>
<@if $arguments as lowercase contain 'disco' @> thirdpartyrepo-version-disco
<@ endif @>
">
{{{
deb <@ $arguments.0 @> VERSION
<@ for $arg in $arguments @>
<@ if $loop.index > 1 @>
<@ if ['disco', 'bionic', 'xenial'] contains $arg @>
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

## Macro=[[Vorlage(Fremdquelle, %%URL, Ubuntuversion(en), Komponente(n)%%)]]
Um aus der [:Fremdquellen: Fremdquelle] zu installieren, muss man
<@ if $arguments as lowercase contain 'trusty' or
      $arguments as lowercase contain 'saucy' or
      $arguments as lowercase contain 'raring' or
      $arguments as lowercase contain 'quantal' or
      $arguments as lowercase contain 'precise' or
      $arguments as lowercase contain 'oneiric' or
      $arguments as lowercase contain 'lucid' or
      $arguments as lowercase contain 'hardy' @> die folgenden [:Paketquellen freischalten:]:
[[Vorlage(Fremd, Quelle)]] 


{{|<class="thirdpartyrepo-outer
<@ if $arguments as lowercase contain 'hardy' @> thirdpartyrepo-version-hardy
<@ endif @>
<@ if $arguments as lowercase contain 'lucid' @> thirdpartyrepo-version-lucid
<@ endif @>
<@ if $arguments as lowercase contain 'oneiric' @> thirdpartyrepo-version-oneiric
<@ endif @>
<@ if $arguments as lowercase contain 'precise' @> thirdpartyrepo-version-precise
<@ endif @>
<@if $arguments as lowercase contain 'quantal' @> thirdpartyrepo-version-quantal
<@ endif @>
<@if $arguments as lowercase contain 'raring' @> thirdpartyrepo-version-raring
<@ endif @>
<@if $arguments as lowercase contain 'raring' @> thirdpartyrepo-version-saucy
<@ endif @>
<@if $arguments as lowercase contain 'saucy' @> thirdpartyrepo-version-trusty
<@ endif @>">
{{{
deb <@ $arguments.0 @> VERSION
<@ for $arg in $arguments @>
<@ if $loop.index > 1 @>
<@ if ['trusty','saucy','raring','quantal','precise','oneiric','lucid','hardy'] contains $arg @>
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

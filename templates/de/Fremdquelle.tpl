## Macro=[[Vorlage(Fremdquelle, %%URL, Ubuntuversion(en), Komponente(n)%%)]]
Um aus der [:Fremdquellen: Fremdquelle] zu installieren, muss man <@ if $arguments as lowercase contain 'precise' or $arguments as lowercase contain 'oneiric' or $arguments as lowercase contain 'natty' or $arguments as lowercase contain 'maverick' or $arguments as lowercase contain 'lucid' or $arguments as lowercase contain 'karmic' or $arguments as lowercase contain 'hardy' or $arguments as lowercase contain 'dapper' @>die folgenden [:Paketquellen freischalten:]:
[[Vorlage(Fremd, Quelle)]]


{{|<class="thirdpartyrepo-outer<@ if $arguments as lowercase contain 'dapper' @> thirdpartyrepo-version-dapper<@ endif @><@ if $arguments as lowercase contain 'hardy' @> thirdpartyrepo-version-hardy<@ endif @><@ if $arguments as lowercase contain 'karmic' @> thirdpartyrepo-version-karmic<@ endif @><@ if $arguments as lowercase contain 'lucid' @> thirdpartyrepo-version-lucid<@ endif @><@ if $arguments as lowercase contain 'maverick' @> thirdpartyrepo-version-maverick<@ endif @><@ if $arguments as lowercase contain 'natty' @> thirdpartyrepo-version-natty<@ endif @><@ if $arguments as lowercase contain 'oneiric' @> thirdpartyrepo-version-oneiric<@ endif @><@ if $arguments as lowercase contain 'precise' @> thirdpartyrepo-version-precise<@ endif @>">
{{{
deb <@ $arguments.0 @> VERSION <@ for $arg in $arguments @><@ if $loop.index > 1 @><@ if ['precise','oneiric','natty','maverick', 'lucid','karmic','hardy','dapper'] contains $arg @><@ else @><@ $arg @> <@ endif @><@ endif @><@ endfor @>
}}}
|}}


<@ else @>unabhängig von der Ubuntu-Version die folgende [:Paketquellen freischalten: Paketquelle freischalten]:
[[Vorlage(Fremd, Quelle)]]
{{{
deb <@ $arguments join_with ' ' @>
}}}
<@ endif @>

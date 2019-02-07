<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:dyn="http://exslt.org/dynamic">

  <!-- result of the last request -->
  <xsl:template match="output">
    <div id="outDlg" class="modal" tabindex="-1" role="dialog">
      <div class="modal-dialog" role="document">
        <div class="modal-content">
          <div class="modal-header">
            <h3 class="modal-title">Arduino&lt;Web&gt;Menu </h3>
            <button type="button" class="close" data-dismiss="modal" aria-label="Close">
              <span aria-hidden="true"><xsl:text disable-output-escaping="yes">X</xsl:text></span>
            </button>
          </div>
          <div class="modal-body">
            <p class="out"><xsl:value-of select="." disable-output-escaping="yes"/></p>
          </div>
          <div class="modal-footer">
            <button type="button" class="btn btn-primary" data-dismiss="modal">Ok</button>
          </div>
        </div>
      </div>
    </div>
  </xsl:template>

  <xsl:template match="*">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="sourceURL">
    <h2 class="sourceURL"><xsl:value-of select="../@host"/></h2>
  </xsl:template>

  <xsl:template match="menuLib/panel">
    <div class="aml_panel"><xsl:apply-templates/></div>
  </xsl:template>

  <xsl:template match="menuLib/panel/tit">
    <h3><xsl:apply-templates/></h3>
  </xsl:template>

  <xsl:template match="menu">
    <ul><xsl:apply-templates/></ul>
  </xsl:template>

  <xsl:template match="op">
    <li class="row aml_op btn btn-lg btn-success">
      <xsl:apply-templates/>
    </li>
  </xsl:template>

  <xsl:template match="p[@t='menu']">
    <xsl:call-template name="linkBtn"/>
  </xsl:template>

  <xsl:template match="p[@t='mn']" mode="pad">
    <xsl:call-template name="linkBtn"/>
  </xsl:template>

  <xsl:template name="linkBtn">
    <xsl:variable name="path">
      <xsl:value-of select="ancestor::*/@r"/>
      <xsl:for-each select="ancestor-or-self::*/@i">/<xsl:value-of select="."/></xsl:for-each>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="@dis='y'">
        <xsl:apply-templates/>
      </xsl:when>
      <xsl:otherwise>
        <a href="/menu?at={$path}">
          <xsl:apply-templates/>
        </a>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="op[ob/p/@t='prompt']|op[ob/p/@t='mn']">
    <xsl:choose>
      <xsl:when test="@dis='y'">
        <li class="row aml_op btn btn-lg btn-disabled">
          <xsl:call-template name="linkBtn"/>
        </li>
      </xsl:when>
      <xsl:otherwise>
        <li class="row aml_op btn btn-lg btn-success linkBtn">
          <xsl:call-template name="linkBtn"/>
        </li>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="op[ob/p/@t='prompt']|op[ob/p/@t='mn']" mode="pad">
    <div class="padOp"><xsl:call-template name="linkBtn"/></div>
  </xsl:template>

  <xsl:template match="op[@pad='y']">
    <li class="row aml_op btn btn-lg btn-success">
      <xsl:apply-templates mode="pad"/>
    </li>
  </xsl:template>

  <!-- toggle field -->
  <xsl:template match="tog">
    <xsl:apply-templates select="node/value" mode="btns"/>
  </xsl:template>

  <!-- toggle field -->
  <xsl:template match="tog" mode="pad">
    <xsl:apply-templates select="node/value" mode="btns"/>
  </xsl:template>

  <xsl:template match="fld" mode="pad">
    <xsl:apply-templates select="."/>
  </xsl:template>

  <!-- toggle option -->
  <xsl:template match="node/value" mode="btns">
    <xsl:variable name="path">
      <xsl:value-of select="ancestor::*/@r"/>
      <xsl:for-each select="ancestor::*/@i">/<xsl:value-of select="."/></xsl:for-each>
    </xsl:variable>
    <a class="aml_btn btn btn-md btn-success" data-grp="{$path}" data-src="{$path}/{position()-1}" href="/menu?at={$path}/{position()-1}">
      <xsl:apply-templates/>
    </a>
  </xsl:template>

  <!-- toggle current option -->
  <xsl:template match="node/value[@selected]" mode="btns">
    <xsl:variable name="path">
      <xsl:value-of select="ancestor::*/@r"/>
      <xsl:for-each select="ancestor::*/@i">/<xsl:value-of select="."/></xsl:for-each>
    </xsl:variable>
    <a class="aml_btn btn btn-md btn-primary" data-grp="{$path}" data-src="{$path}/{position()-1}" href="/menu?at={$path}/{position()-1}">
      <xsl:apply-templates/>
    </a>
  </xsl:template>

  <!-- ignorered stuff -->
  <xsl:template name="ignore" match="cur|fv"/>
  <xsl:template name="ignorePad" match="cur|fv" mode="pad"/>

  <!-- <xsl:template match="fld[../p[@t='f']]">
    <xsl:variable name="path">
      <xsl:value-of select="ancestor::*/@r"/>
      <xsl:for-each select="ancestor::*/@i">/<xsl:value-of select="."/></xsl:for-each>
    </xsl:variable>
    <xsl:element name="input">
      <xsl:attribute name="id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../@i"/></xsl:attribute>
      <xsl:attribute name="data-slider-id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../@i"/></xsl:attribute>
      <xsl:attribute name="type">text</xsl:attribute>
      <xsl:attribute name="class">slider ledField</xsl:attribute>
      <xsl:attribute name="data-slider-min"><xsl:value-of select="@l"/></xsl:attribute>
      <xsl:attribute name="data-slider-max"><xsl:value-of select="@h"/></xsl:attribute>
      <xsl:attribute name="data-slider-step"><xsl:value-of select="@t"/></xsl:attribute>
      <xsl:attribute name="data-slider-value"><xsl:apply-templates/></xsl:attribute>
      <xsl:attribute name="value"><xsl:apply-templates/></xsl:attribute>
      <xsl:attribute name="data-src"><xsl:value-of select="$path"/></xsl:attribute>
      <xsl:attribute name="onchange">window.location='/menu?at='+getAttribute("data-src")+'/'+value</xsl:attribute>
    </xsl:element>
  </xsl:template> -->

  <xsl:template match="fld[../u='%']">
    <xsl:variable name="path">
      <xsl:value-of select="ancestor::*/@r"/>
      <xsl:for-each select="ancestor::*/@i">/<xsl:value-of select="."/></xsl:for-each>
    </xsl:variable>
    <xsl:element name="input">
      <xsl:attribute name="id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../@i"/></xsl:attribute>
      <xsl:attribute name="data-slider-id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../@i"/></xsl:attribute>
      <xsl:attribute name="type">text</xsl:attribute>
      <xsl:attribute name="class">slider ledField</xsl:attribute>
      <xsl:attribute name="data-slider-min"><xsl:value-of select="@l"/></xsl:attribute>
      <xsl:attribute name="data-slider-max"><xsl:value-of select="@h"/></xsl:attribute>
      <xsl:attribute name="data-slider-step"><xsl:value-of select="@t"/></xsl:attribute>
      <xsl:attribute name="data-slider-value"><xsl:apply-templates/></xsl:attribute>
      <xsl:attribute name="value"><xsl:apply-templates/></xsl:attribute>
      <xsl:attribute name="data-src"><xsl:value-of select="$path"/></xsl:attribute>
      <xsl:attribute name="onchange">window.location='/menu?at='+getAttribute("data-src")+'/'+value</xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="fld">
    <xsl:call-template name="fld">
      <xsl:with-param name="at">
        <xsl:value-of select="ancestor::*/@r"/>
        <xsl:for-each select="ancestor::*/@i">/<xsl:value-of select="."/></xsl:for-each>
      </xsl:with-param>
      <xsl:with-param name="type"><xsl:value-of select="../p/@t"/></xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="fld">
    <xsl:param name="at"/>
    <xsl:param name="type"/>
    <xsl:variable name="ln">
      <xsl:choose>
        <xsl:when test="@t>0">
          <xsl:value-of select="(@h - @l) div @t"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="(@h - @l) div @s"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="szBase">
      <xsl:choose>
        <xsl:when test="@h&lt;10 and @l&gt;-10">1</xsl:when>
        <xsl:when test="@h&lt;100 and @l&gt;-100">2</xsl:when>
        <xsl:when test="@h&lt;1000 and @l&gt;-1000">3</xsl:when>
        <xsl:when test="@h&lt;10000 and @l&gt;-10000">4</xsl:when>
        <xsl:otherwise>8</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="sz">
      <xsl:choose>
        <xsl:when test="$type='f' and (@s&lt;1 or @t&lt;0)"><xsl:value-of select="$szBase+3"/></xsl:when>
        <xsl:when test="@l&lt;0"><xsl:value-of select="$szBase+1"/></xsl:when>
        <xsl:otherwise><xsl:value-of select="$szBase"/></xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$ln>=10000 and $ln&lt;=1000"><!-- disabled in pactice, with a large value -->
        <xsl:element name="input">
          <xsl:attribute name="id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../@i"/></xsl:attribute>
          <xsl:attribute name="data-slider-id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../@i"/></xsl:attribute>
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="class">slider ledField</xsl:attribute>
          <xsl:attribute name="data-slider-min"><xsl:value-of select="@l"/></xsl:attribute>
          <xsl:attribute name="data-slider-max"><xsl:value-of select="@h"/></xsl:attribute>
          <xsl:attribute name="data-slider-step"><xsl:value-of select="@t"/></xsl:attribute>
          <xsl:attribute name="data-slider-value"><xsl:apply-templates/></xsl:attribute>
          <xsl:attribute name="value"><xsl:apply-templates/></xsl:attribute>
          <!-- <xsl:attribute name="data-src"><xsl:value-of select="$path"/></xsl:attribute> -->
          <xsl:attribute name="onchange">window.location='/menu?at='+getAttribute("data-src")+'/'+value</xsl:attribute>
        </xsl:element>
        <!-- some fancy slider.. not working yet -->
        <!-- <xsl:element name="input">
          <xsl:attribute name="id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../@i"/></xsl:attribute>
          <xsl:attribute name="data-slider-id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../@i"/></xsl:attribute>
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="data-slider-min"><xsl:value-of select="@l"/></xsl:attribute>
          <xsl:attribute name="data-slider-max"><xsl:value-of select="@h"/></xsl:attribute>
          <xsl:attribute name="data-slider-step"><xsl:value-of select="@t"/></xsl:attribute>
          <xsl:attribute name="data-slider-value"><xsl:apply-templates/></xsl:attribute>
        </xsl:element> -->
        <!-- standard slider -->
        <!-- <xsl:element name="input">
          <xsl:copy-of select="node/@*"/>
          <xsl:copy-of select="../../p/@i"/>
          <xsl:attribute name="class">menu-range-fld</xsl:attribute>
          <xsl:attribute name="type">range</xsl:attribute>
          <xsl:attribute name="value"><xsl:apply-templates/></xsl:attribute>
          <xsl:attribute name="data-src"><xsl:value-of select="$at"/></xsl:attribute>
          <xsl:attribute name="onchange">window.location='/menu?at='+getAttribute("data-src")+'/'+value</xsl:attribute>
        </xsl:element> -->
      </xsl:when>
      <xsl:otherwise>
      <xsl:element name="input">
          <xsl:attribute name="id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../@i"/></xsl:attribute>
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="size"><xsl:value-of select="$sz"/></xsl:attribute>
          <xsl:if test="$sz&lt;8">
            <xsl:attribute name="maxlength"><xsl:value-of select="$sz"/></xsl:attribute>
          </xsl:if>
          <xsl:attribute name="value"><xsl:apply-templates/></xsl:attribute>
          <xsl:attribute name="class">form-control</xsl:attribute>
          <xsl:attribute name="data-src"><xsl:value-of select="$at"/></xsl:attribute>
          <xsl:attribute name="onchange">window.location='/menu?at='+getAttribute("data-src")+'/'+value</xsl:attribute>
        </xsl:element>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="select|choose">
    <xsl:apply-templates select="node"/>
  </xsl:template>

  <xsl:template match="node">
    <xsl:element name="select">
      <xsl:attribute name="id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../../../p/@i"/></xsl:attribute>
      <xsl:attribute name="class">form-control</xsl:attribute>
      <xsl:attribute name="data-src"><xsl:value-of select="../../../../@r"/>/<xsl:value-of select="../../../@i"/></xsl:attribute>
      <xsl:attribute name="onchange">window.location.href='/menu?at='+getAttribute('data-src')+'/'+selectedIndex</xsl:attribute>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="node/value">
    <xsl:element name="option">
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="fv[../p[@t='textField']]">
    <xsl:element name="input">
    <xsl:attribute name="id">fld_<xsl:value-of select="/menuLib/panel/@id"/>_<xsl:value-of select="../p/@i"/></xsl:attribute>
    <xsl:attribute name="value"><xsl:value-of select="."/></xsl:attribute>
    <xsl:attribute name="class">form-control</xsl:attribute>
    <xsl:attribute name="data-src"><xsl:value-of select="../../../@r"/>/<xsl:value-of select="../../@i"/></xsl:attribute>
    <xsl:attribute name="onchange">window.location='/menu?at='+getAttribute("data-src")+'/'+value</xsl:attribute>
    </xsl:element>
  </xsl:template>

</xsl:stylesheet>

<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- <xsl:output method="html" encoding="utf-8" indent="yes" /> -->

  <xsl:include href="menu.xslt"/>
  <xsl:include href="bootstrap.xslt"/>

  <!-- uTC360 specifics translations /////////////////////////////////////// -->

  <!-- special case for exit, we want to go to main page -->
  <xsl:template match="/menuLib/panel/menu[@r='']/op[@i='4']">
    <li class="aml_op btn btn-lg btn-success linkBtn">
      <a href="/{/menuLib/sourceURL/@ver}index.html"><xsl:apply-templates/></a>
    </li>
  </xsl:template>

  <!-- extras on light/backup menu with download link -->
  <xsl:template match="/menuLib/panel/menu[@r='/0/7']/op[position()&lt;6]">
    <li class="row aml_op btn btn-lg btn-success">
      <div class="col-xs-10"><xsl:call-template name="linkBtn"/></div>
      <a class="col-xs-2" href="/slot{@i+1}" title="Download">
        <img src="/img/download.png" alt="download"/>
      </a>
    </li>
  </xsl:template>

  <!-- extras on light/restore menu with upload form -->
  <xsl:template match="/menuLib/panel/menu[@r='/0/8']/op[position()&lt;6]">
    <xsl:variable name="path">
      <xsl:value-of select="ancestor::*/@r"/>
      <xsl:for-each select="ancestor-or-self::*/@i">/<xsl:value-of select="."/></xsl:for-each>
    </xsl:variable>
    <xsl:variable name="ed">
      <xsl:choose>
        <xsl:when test="@dis='y'">btn-disabled</xsl:when>
        <xsl:otherwise>btn-success</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <li class="row aml_op btn btn-lg {$ed}">
      <div class="col-xs-10"><xsl:call-template name="linkBtn"/></div>
      <div class="col-xs-2" onclick="$('#upDlg{@i}').modal();" title="upload"><img src="/img/upload.png"/></div>
      <div id="upDlg{@i}" class="modal" tabindex="-1" role="dialog">
        <form method="post" enctype="multipart/form-data" action="/menu?at={$path}" class="upForm">
          <div class="form-group">
            <label for="file">Light definition</label>
            <input type="file" name="fn" id="file" class="form-control form-control-lg form-control-file"/>
          </div>
          <div class="form-group">
            <button type="submit" class="btn btn-secondary btn-space" data-dismiss="modal">Cancel</button>
            <button type="submit" class="btn btn-primary btn-space">Upload</button>
          </div>
        </form>
      </div>
    </li>
  </xsl:template>

  <!-- dont print % unit when using sliders -->
  <xsl:template match="/menuLib/panel/menu[@r='/0']/op/ob/panel/menu/op[@i='1']/ob/u" mode="pad"/>
  <xsl:template match="/menuLib/panel/menu[@r='/0']/op/ob/op[@i='1']/ob/u" mode="pad"/>

  <!-- draw a clock icon in replacement of [Auto schedulle] option -->
  <xsl:template match="/menuLib/panel/menu[@r='/0']/op/ob/panel/menu/op[@i='2']/ob/p">
    <xsl:variable name="path">
      <xsl:value-of select="ancestor::*/@r"/>
      <xsl:for-each select="ancestor-or-self::*/@i">/<xsl:value-of select="."/></xsl:for-each>
    </xsl:variable>
    <div class="padOp"><a href="/menu?at={$path}">
      <img src="/img/clock.png" alt="{.}" title="{.}" height="32px"/>
    </a></div>
  </xsl:template>
  <xsl:template match="/menuLib/panel/menu[@r='/0']/op/ob/op[@i='2']/ob/p">
    <xsl:variable name="path">
      <xsl:value-of select="ancestor::*/@r"/>
      <xsl:for-each select="ancestor-or-self::*/@i">/<xsl:value-of select="."/></xsl:for-each>
    </xsl:variable>
    <div class="padOp"><a href="/menu?at={$path}">
      <img src="/img/clock.png" alt="{.}" title="{.}" height="32px"/>
    </a></div>
  </xsl:template>

</xsl:stylesheet>

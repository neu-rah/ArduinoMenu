<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:dyn="http://r-site.net"
	xmlns:msxsl="urn:schemas-microsoft-com:xslt"
	extension-element-prefixes="dyn msxsl">

  <!-- you can host aux files on external server
    if it is the case set this variable to the server address:port
    this is usefull for develop/debug
    to use external server the browser must be intructed to accept cross domain files
  -->
  <xsl:variable name="auxFilesSrc"></xsl:variable>

  <xsl:template match="/">
    <html>
      <head profile="http://www.w3.org/2005/10/profile">
        <meta charset="utf-8"/>
        <meta http-equiv="X-UA-Compatible" content="IE=edge"/>
        <meta name="viewport" content="width=device-width, initial-scale=1"/>
        <!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->
        <meta name="description" content="ArduinoMenu Example IOT/OTA for ESP8266"/>
        <meta name="author" content="Rui Azevedo ruihfazevedo@gmail.com (www.r-site.net)"/>
        <title>ArduinoMenu library OTA</title>
        <link rel="icon" type="image/png" href="/logo.png"/>
        <script src="https://code.jquery.com/jquery-3.1.1.slim.min.js" integrity="sha256-/SIrNqv8h6QGKDuNoLGA4iret+kyesCkHGzVUUV0shc=" crossorigin="anonymous"></script>
        <link href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous"/>
        <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js" integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa" crossorigin="anonymous"></script>
        <!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->
        <!--[if lt IE 9]>
          <script src="https://oss.maxcdn.com/html5shiv/3.7.3/html5shiv.min.js"></script>
          <script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
        <![endif]-->
        <!--link rel="stylesheet" type="text/css" href="/r-site.css"-->
        <link rel="stylesheet" type="text/css" href="{$auxFilesSrc}/menu.css"/>
        <script type="text/javascript" src="{$auxFilesSrc}/r-site.js" ></script>
      </head>
      <body class="ArduinoMenu">
        <div class="site-wrapper">
          <div class="site-wrapper-inner">
            <div class="cover-container">
              <div class="masthead clearfix">
                <div class="inner">
                  <h3 class="masthead-brand">ArduinoMenu library</h3>
                  <nav>
                    <ul class="nav masthead-nav">
                      <li class="active"><a href="https://github.com/neu-rah/ArduinoMenu">Home</a></li>
                      <!--li><a href="#">Features</a></li-->
                      <li><a href="http://www.r-site.net">Contact</a></li>
                    </ul>
                  </nav>
                </div>
              </div>

              <div class="inner cover">
                <h2><img src="logo.png" height="48px"/>on ESP8266 (dev mode!)</h2>
                <xsl:apply-templates/>
              </div>

              <div class="mastfoot">
                <div class="inner">
                  <p>ArduinoMenu library <a href="https://github.com/neu-rah/ArduinoMenu">https://github.com/neu-rah/ArduinoMenu</a>.</p>
                </div>
              </div>
            </div>
          </div>
        </div>
      </body></html>
  </xsl:template>

  <xsl:template match="panel">
    <div>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </div>
  </xsl:template>

  <xsl:template match="title">
    <h2>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </h2>
  </xsl:template>

  <xsl:template match="menu">
    <ul>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </ul>
  </xsl:template>

  <xsl:template match="select/node">
    <select>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </select>
  </xsl:template>

  <xsl:template match="select/node/value">
    <xsl:element name="option">
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="op[opBody/field]|op[opBody/select]">
    <li>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </li>
  </xsl:template>

  <xsl:template match="op">
    <li><a>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </a></li>
  </xsl:template>

  <xsl:template match="field">
    <li>
      <xsl:element name="input">
        <xsl:copy-of select="node/@*"/>
        <xsl:copy-of select="../../@data-idx"/>
        <xsl:attribute name="class">menu-range-field</xsl:attribute>
        <xsl:attribute name="type">range</xsl:attribute>
        <xsl:attribute name="value"><xsl:apply-templates/></xsl:attribute>
      </xsl:element>
    </li>
  </xsl:template>

  <xsl:template match="*">
    <xsl:apply-templates/>
  </xsl:template>

</xsl:stylesheet>

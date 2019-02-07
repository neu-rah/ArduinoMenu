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
  <!-- <xsl:variable name="auxFilesSrc"></xsl:variable> -->
  <xsl:variable name="auxFilesSrc">
    <xsl:choose>
      <xsl:when test="/menuLib/@debug='yes'">http://neurux:8080/</xsl:when>
      <xsl:otherwise></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:template match="/">
    <html>
      <head>
        <meta charset="utf-8"/>
        <meta http-equiv="X-UA-Compatible" content="IE=edge"/>
        <meta name="viewport" content="width=device-width, initial-scale=1"/>
        <!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->
        <meta name="description" content="ArduinoMenu Library auto generated web menu"/>
        <meta name="author" content="Rui Azevedo ruihfazevedo@gmail.com (https://github.com/neu-rah)"/>
        <title>ArduinoMenu&lt;web&gt;</title>
        <!-- <script src="{$auxFilesSrc}/jquery-3.3.1.min.js"></script> -->
        <script
          src="https://code.jquery.com/jquery-3.3.1.slim.min.js"
          integrity="sha256-3edrmyuQ0w65f8gfBsqowzjJe2iM6n0nKciPUp8y+7E="
          crossorigin="anonymous"></script>

        <!-- Latest compiled and minified CSS -->
        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous"/>

        <!-- Optional theme -->
        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css" integrity="sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp" crossorigin="anonymous"/>

        <!-- Latest compiled and minified JavaScript -->
        <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js" integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa" crossorigin="anonymous"></script>

        <!-- <link href="{$auxFilesSrc}/bootstrap.min.css" rel="stylesheet"/>
        <script src="{$auxFilesSrc}/bootstrap.min.js"></script> -->
        <link href="{$auxFilesSrc}/bootstrap-slider.min.css" rel="stylesheet"/>
        <script src="{$auxFilesSrc}/bootstrap-slider.min.js"></script>
        <link rel="stylesheet" type="text/css" href="{$auxFilesSrc}/{/menuLib/sourceURL/@ver}menu.css"/>
        <script src="{$auxFilesSrc}/{/menuLib/sourceURL/@ver}r-site.js"></script>
        <link rel="icon" type="image/png" href="/img/icon.png"/>
      </head>
      <body class="ArduinoMenu">
        <div class="site-wrapper">
          <div class="site-wrapper-inner">
            <div class="cover-container">
              <div class="masthead clearfix">
                <div class="wrap">
                <div class="inner">
                  <h1 class="masthead-brand">
                    <a href="http://r-site.net/" target="_blank"><img src="/img/logo.png" height="72px" alt="r-site.net"/></a>
        				  </h1>
                          <nav>
                            <ul class="nav masthead-nav">
                              <li><a href="/" id="start">Home</a></li>
                              <li class="active"><a href="/menu?at=/" id="menu">Menu</a></li>
                              <li><a href="https://github.com/neu-rah/ArduinoMenu" id="repo" target="_blank">Repo</a></li>
                              <li><a href="http://www.r-site.net/site/struct.asp?at=//menu[@id=%27main%27]/*[5]" id="contact" target="_blank">Contact</a></li>
                            </ul>
                          </nav>
                </div>
                </div>
              </div>
              <div class="inner cover">
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
      </body>
    </html>
  </xsl:template>

</xsl:stylesheet>

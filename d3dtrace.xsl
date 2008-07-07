<?xml version="1.0"?>

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" />

	<xsl:template match="/trace">
		<html>
			<head>
				<title>D3D Trace</title>
				<link rel="stylesheet" type="text/css" href="d3dtrace.css"/>
			</head>
			<body>
				<ul>
					<xsl:apply-templates/>
				</ul>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="call">
		<li>
			<span class="fun">
				<xsl:value-of select="@name"/>
			</span>
			<xsl:text>(</xsl:text>
			<xsl:apply-templates select="param"/>
			<xsl:text>)</xsl:text>
			<xsl:apply-templates select="return"/>
		</li>
	</xsl:template>

	<xsl:template match="param">
		<span class="typ">
			<xsl:value-of select="@type"/>
			<xsl:text> </xsl:text>
		</span>
		<span class="var">
			<xsl:value-of select="@name"/>
		</span>
		<xsl:text> = </xsl:text>
		<span class="lit">
			<xsl:value-of select="."/>
		</span>
		<xsl:if test="position() != last()">
			<xsl:text>, </xsl:text>
		</xsl:if>
	</xsl:template>

	<xsl:template match="return">
		<xsl:text> = </xsl:text>
		<span class="lit">
			<xsl:value-of select="."/>
		</span>
	</xsl:template>

</xsl:transform>

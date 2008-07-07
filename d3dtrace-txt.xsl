<?xml version="1.0"?>

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:strip-space elements="*" />

	<xsl:template match="/trace">
		<xsl:apply-templates/>
	</xsl:template>

	<xsl:template match="call">
		<xsl:value-of select="@name"/>
		<xsl:text>(</xsl:text>
		<xsl:apply-templates select="param"/>
		<xsl:text>)</xsl:text>
		<xsl:apply-templates select="return"/>
		<xsl:text>&#10;</xsl:text>
	</xsl:template>

	<xsl:template match="param">
		<xsl:value-of select="@name"/>
		<xsl:text> = </xsl:text>
		<xsl:value-of select="."/>
		<xsl:if test="position() != last()">
			<xsl:text>, </xsl:text>
		</xsl:if>
	</xsl:template>

	<xsl:template match="return">
		<xsl:text> = </xsl:text>
		<xsl:value-of select="."/>
	</xsl:template>

</xsl:transform>

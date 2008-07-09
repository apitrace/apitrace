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
		<xsl:apply-templates select="arg"/>
		<xsl:text>)</xsl:text>
		<xsl:apply-templates select="ret"/>
		<xsl:text>&#10;</xsl:text>
	</xsl:template>

	<xsl:template match="arg|elem">
		<xsl:value-of select="@name"/>
		<xsl:text> = </xsl:text>
		<xsl:call-template name="compound"/>
		<xsl:if test="position() != last()">
			<xsl:text>, </xsl:text>
		</xsl:if>
	</xsl:template>

	<xsl:template match="ret">
		<xsl:text> = </xsl:text>
		<xsl:call-template name="compound"/>
	</xsl:template>

	<xsl:template match="ref">
		<xsl:choose>
			<xsl:when test="*">
				<xsl:text>&amp;</xsl:text>
				<xsl:call-template name="compound"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="@addr"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="compound">
		<xsl:choose>
			<xsl:when test="elem">
				<xsl:text>{</xsl:text>
				<xsl:apply-templates />
				<xsl:text>}</xsl:text>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:transform>

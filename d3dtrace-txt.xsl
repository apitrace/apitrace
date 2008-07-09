<?xml version="1.0"?>

<!--

Copyright 2008 Jose Fonseca, Tungsten Graphics, Inc.

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

--!>

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
